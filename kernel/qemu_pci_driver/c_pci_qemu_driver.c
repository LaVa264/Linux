#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/cdev.h>

#define TYPE_PCI_CUSTOM_DEVICE  "c_pci_dev"
#define DEVICE_VENDOR_ID        0x1234
#define DEVICE_DEVICE_ID        0xABCD

#define REG_OP1                 0x10
#define REG_OP2                 0x14
#define REG_OPCODE              0x18
#define REG_RESULT              0x20
#define REG_ERROR               0x24
#define OPCODE_ADD              0x00
#define OPCODE_MUL              0x01
#define OPCODE_DIV              0x02
#define OPCODE_SUB              0x03

#define DMA_REG_CMD             0x00
#define DMA_REG_SRC             0x04
#define DMA_REG_DST             0x08
#define DMA_REG_LEN             0x0C

/**
 * our CMD register:
 * Bit 0 is run DMA or not.
 * Bit 1 are DMA direction: to device or from device.
 */
#define DMA_CMD_RUN                 1
#define DMA_DIRECTION_TO_DEVICE     0
#define DMA_DIRECTION_FROM_DEVICE   1

#define DMA_GET_DIR(cmd) ((cmd & 0b10) >> 1)

#define DEVICE_NAME TYPE_PCI_CUSTOM_DEVICE

#define __pr_info(fmt, arg...) pr_info("%s():" fmt, __FUNCTION__, ##arg)
#define __pr_err(fmt, arg...) pr_err("%s():" fmt, __FUNCTION__, ##arg)


struct c_pci_dev {
    struct pci_dev *_dev;
    struct class *_cls;
    void __iomem *bar_2_ptr;
    int _major;
} _dev;

static struct pci_device_id dev_ids[] = {
    {PCI_DEVICE(DEVICE_VENDOR_ID, DEVICE_DEVICE_ID)},
    {}
};
MODULE_DEVICE_TABLE(pci, dev_ids);

static int _mmap(struct file *file, struct vm_area_struct *vma)
{
    int res = 0;

    /* VM Area offset will point to first page of PCI DMA (physical addr).
     * pci_resource_start() return start address od PCI BAR.
     * We shift `PAGE_SHIFT` bits the address to right to get the page number.
     **/
    vma->vm_pgoff = pci_resource_start(_dev._dev, 0) >> PAGE_SHIFT;

    /* We map user VMA to the BAR. */
    res = io_remap_pfn_range(vma,
                             vma->vm_start,
                             vma->vm_pgoff,
                             vma->vm_end - vma->vm_start,
                             vma->vm_page_prot);
    if (res) {
        __pr_err("Failed to map PCI BAR 0 to user VMA: %d", res);
        res = -res;
        goto exit;
    }

exit:
    return res;
}

static int _open(struct inode *inode, struct file *f);
static int _release(struct inode *inode, struct file *f);
static ssize_t _read(struct file *f, char __user *p, size_t size, loff_t *offset);
static ssize_t _write(struct file *f, const char __user *p, size_t size, loff_t *offset);

static struct file_operations f_ops = {
    .read = _read,
    .write = _write,
    .open = _open,
    .release = _release,
    .mmap = _mmap,
};

static int _probe(struct pci_dev *dev, const struct pci_device_id *id)
{
    int res = 0;
    void __iomem *bar_0_ptr = NULL;
    void __iomem *bar_1_ptr = NULL;

    /* 1. Enable PCI device. */
    res = pcim_enable_device(dev);
    if (res < 0) {
        pr_err("%s(): Failed to enable PCI device.\n", __FUNCTION__);
        goto exit;
    }

    pci_set_master(dev);

    /* Map device's memory regions. */
    bar_0_ptr = pcim_iomap(dev, 0, pci_resource_len(dev, 0));
    if (bar_0_ptr == NULL)
    {
        pr_err("%s(): Failed to map mem region 0.\n", __FUNCTION__);
        res = -ENODEV;
        goto exit;
    }

    pr_info("%s(): Region 0 length: %d \n", __FUNCTION__, pci_resource_len(dev, 0));

    bar_1_ptr = pcim_iomap(dev, 1, pci_resource_len(dev, 1));
    if (bar_1_ptr == NULL)
    {
        pr_err("%s(): Failed to map mem region 1.\n", __FUNCTION__);
        res = -ENODEV;
        goto exit;
    }

    pr_info("%s(): Region 1 length: %d \n", __FUNCTION__, pci_resource_len(dev, 1));

    _dev.bar_2_ptr = pcim_iomap(dev, 2, pci_resource_len(dev, 2));
    if ( _dev.bar_2_ptr == NULL)
    {
        pr_err("%s(): Failed to map mem region 2.\n", __FUNCTION__);
        res = -ENODEV;
        goto exit;
    }

    pr_info("%s(): Region 2 length: %d \n", __FUNCTION__, pci_resource_len(dev, 2));

    /* 3. Test math operators. */
    iowrite32((u32)1, bar_0_ptr + REG_OP1);
    iowrite32((u32)2, bar_0_ptr + REG_OP2);
    iowrite32((u32)OPCODE_ADD, bar_0_ptr + REG_OPCODE);

    mdelay(1);

    pr_info("%s(): Read result from BAR0: %d\n",
            __FUNCTION__,
            ioread32(bar_0_ptr + REG_RESULT));

    /* 4. Expose our driver. */
    _dev._dev = dev;
    _dev._major = register_chrdev(0, DEVICE_NAME, &f_ops);
    if (_dev._major < 0)
    { // Registration failed.
        pr_alert("Registering char device failed with %d\n",  _dev._major);
        res = _dev._major;
        goto exit;
    }

    /* Create a struct class structure.
     * @owner: pointer to the module that is to `own` this struct class.
     * @name: pointer to a string for the name of this class.
     */
     _dev._cls = class_create(DEVICE_NAME);
    if (IS_ERR_OR_NULL(_dev._cls)) {
        res = PTR_ERR(_dev._cls);
        pr_err("%s(): Failed to create class: %d\n", __FUNCTION__, res);
        goto release_dev;
    }

    /* Creates a device and registers it with sysfs.
     * @class: pointer to the struct class that this device should be registered to.
     * @parent: pointer to the parent struct device of this new device, if any.
     * @devt: the dev_t for the char device to be added.
     * @drvdata: the data to be added to the device for callbacks.
     * @fmt: string for the device's name.
     * @...: variable arguments.
     */
    device_create(_dev._cls, NULL, MKDEV(_dev._major, 0), NULL, DEVICE_NAME);

    __pr_info("Device created on /dev/%s.\n", DEVICE_NAME);

    return 0;

release_dev:
    unregister_chrdev(_dev._major, DEVICE_NAME);
exit:
    return res;
}

static void _remove(struct pci_dev *dev)
{
    __pr_info("invoked.\n");
    device_destroy(_dev._cls, MKDEV(_dev._major, 0));
    class_destroy(_dev._cls);
    unregister_chrdev(_dev._major, DEVICE_NAME);
}

static struct pci_driver _driver = {
    .name = TYPE_PCI_CUSTOM_DEVICE,
    .probe = _probe,
    .remove = _remove,
    .id_table = dev_ids
};

static int _dma_transfer(struct c_pci_dev* _dev,
                         void *buffer,
                         int len,
                         dma_addr_t address,
                         uint8_t dir)
{
    __pr_info("invoked.\n");

    /* Map virtual memory `buffer` to the DMA device. */
    dma_addr_t buffer_dma_addr = dma_map_single(&_dev->_dev->dev,
                                                buffer,
                                                len,
                                                dir);

    /* We configure DMA controller via registers first. */
    iowrite32(len, _dev->bar_2_ptr + DMA_REG_LEN);

    if (dir == DMA_DIRECTION_FROM_DEVICE) {
        /* We read from the device, so dest will be our virtual memory buffer.
         * Source data will be device address. In our case is offset from user.
         */

        iowrite32(buffer_dma_addr, _dev->bar_2_ptr + DMA_REG_DST);
        iowrite32(address, _dev->bar_2_ptr + DMA_REG_SRC);
    } else if (dir == DMA_DIRECTION_TO_DEVICE) {
        iowrite32(buffer_dma_addr, _dev->bar_2_ptr + DMA_REG_SRC);
        iowrite32(address, _dev->bar_2_ptr + DMA_REG_DST);
    } else {
        __pr_err("Invalid DIR\n");
        goto exit;
    }

    /* We send run command let DMA controller read/write to the buffer. */
    iowrite32(DMA_CMD_RUN | (dir << 1), _dev->bar_2_ptr + DMA_REG_CMD);

exit:
    dma_unmap_single(&_dev->_dev->dev, buffer_dma_addr, len, dir);
    return 0;
}

static int _open(struct inode *inode, struct file *f)
{
    __pr_info("invoked.\n");

    return 0;
}

static int _release(struct inode * inode, struct file *f)
{
    __pr_info("invoked.\n");
    return 0;
}

static ssize_t _read(struct file *f, char __user *p, size_t size, loff_t *offset)
{
    __pr_info("invoked.\n");

    void *buf = NULL;
    int user_len = 0;
    int number_of_byte_not_transferred = 0;
    if (size + *offset < pci_resource_len(_dev._dev, 1)) {
        user_len = size;
    } else {
        user_len = pci_resource_len(_dev._dev, 1) - *offset;
    }

    buf = kmalloc(user_len, GFP_ATOMIC);

    /* We read from DMA to kernel buffer. */
    _dma_transfer(&_dev, buf, user_len, *offset, DMA_DIRECTION_FROM_DEVICE);

    /* Wait a second for device do it's job. In real word, we can do something
     * and receive DMA callback done. In our driver, we do some trick to wait
     * the device done. */
    mdelay(5);

    /* Copy from kernel buffer to user space. */
    number_of_byte_not_transferred = copy_to_user(p, buf, user_len);

    kfree(buf);

    *offset += user_len - number_of_byte_not_transferred;
    return user_len - number_of_byte_not_transferred;
}

static ssize_t _write(struct file *f, const char __user *p, size_t size, loff_t *offset)
{
    __pr_info("invoked.\n");

    void *buf = NULL;
    int user_len = 0;
    int number_of_byte_not_transferred = 0;

    if (size + *offset < pci_resource_len(_dev._dev, 1)) {
        user_len = size;
    } else {
        user_len = pci_resource_len(_dev._dev, 1) - *offset;
    }

    buf = kmalloc(user_len, GFP_ATOMIC);

    number_of_byte_not_transferred = copy_from_user(buf, p, user_len);
    user_len -= number_of_byte_not_transferred;

    /* Start transfer data from kernel buffer to device memory. */
    _dma_transfer(&_dev, buf, user_len, *offset, DMA_DIRECTION_TO_DEVICE);

    mdelay(5);
    kfree(buf);

    *offset += user_len;
    return user_len;
}

module_pci_driver(_driver);
MODULE_LICENSE("GPL");
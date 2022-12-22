#ifndef _INTERFACE_CONFIG_H_
#define _INTERFACE_CONFIG_H_

/** Serial Interface configurations */
#define USB_SERIAL_LINE_SIZE    (32U)
#define USB_SERIAL_SAVED_CMDS   (8U)

/** Serial Interface memory region */
#define MEM_REG_OFFSET_ITRFCE   (0U)
#define MEM_REG_LENGTH_ITRFCE   ((USB_SERIAL_SAVED_CMDS * USB_SERIAL_LINE_SIZE) + (2 * sizeof(int16_t)))

#endif /* _INTERFACE_CONFIG_H_ */
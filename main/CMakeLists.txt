# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "font8.c" "font12.c" "font16.c" "font20.c" "font24.c" "lcd_driver.c" "sd_spi_device.c" "eeprom_driver.c" "inet_protocols_driver.c" "main.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()

idf_component_register(
        SRCS "../../../../sfud/src/sfud.c" "../../../../sfud/src/sfud_sfdp.c" "sfud_port.c"
        INCLUDE_DIRS "." "../../../../sfud/inc"
	REQUIRES driver spi_flash esp_timer 
)

idf_component_register(SRCS "ota.c" "main.c"
                            "wifi.c"
                            "packed_fs.c"
                            "mongoose.c"
                            "lcd.c"
                            "lvgl_demo_ui.c" 
                        INCLUDE_DIRS "../include"
                        INCLUDE_DIRS "."
                        INCLUDE_DIRS "../managed_components")
# component_compile_options(-DHTTP_URL="http://0.0.0.0:80")
component_compile_options(-DHTTPS_URL="https://0.0.0.0:443")

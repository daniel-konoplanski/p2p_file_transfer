add_library(ui STATIC console_user_interface.cpp progress_bar/progress_bar.cpp)

target_link_libraries(ui PUBLIC indicators::indicators)

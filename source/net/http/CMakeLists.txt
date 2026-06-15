set(Http_SRCS
    HttpServer.cc
    HttpResponse.cc
    HttpContext.cc
)

add_library(muduo_http ${Http_SRCS})
target_link_libraries(muduo_http muduo_net)

add_executable(HttpServer HttpServer_example.cc)
target_link_libraries(HttpServer muduo_http)
#include <cstdio>

extern "C" {
    #include <libavdevice/avdevice.h>
    #include <libavformat/avformat.h>
}

int main() {
    av_register_all();
    avformat_network_init();
    avdevice_register_all();

    AVOutputFormat* output_device = av_output_video_device_next(nullptr);




    while (output_device != nullptr) {
        printf("%s: %s\n", output_device->name, output_device->long_name);
        
        if (strcmp(output_device->name, "sdl,sdl2") == 0) {
            break;
        }
        output_device = av_output_video_device_next(output_device);
    }

    printf("Hello world!\n %s\n", output_device->name);

    AVFormatContext* output_context = nullptr;

    int result =  avformat_open_input(&output_context,
        "rtp://@127.0.0.1:4242",
        nullptr,
        nullptr);

    printf("%d Starting %s\n", result, output_context->iformat->name);

    output_context->oformat = output_device;

    printf("%d\n", avformat_write_header(output_context, nullptr));
}
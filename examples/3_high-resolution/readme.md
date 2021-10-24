## ArduCAM high resolution image example

### How to use

1. hardware: connect the pins as mentioned in the readme file under the root folder 

2. software

go to `network_common.h` and set the constants 

| name   | description   |
| ------ | ------------- |
| MYSSID | Wifi SSID     |
| MYPWD  | Wifi password |
| MYMQTT | mqtt url      |

3. nodered

In nodered, import the "ov2640_both_low_and_high_res_images" flow. You will need to install one community-contributed node called [message-counter](https://flows.nodered.org/node/node-red-contrib-message-counter).

### multiple message workaround for high resolution images

The OV2640 module supports high resolution images up to 1600x1200. Even with JPEG compression, the binary size will be too large to hold in the memory.

> resolution: 1600x1200
>
> pixel format: RGB565, each pixel takes 2 bytes
>
> bitmap size: 1600x1200x2 = 3.66MB
>
> JPEG compressed binary size: bitmap size x 4% ~= 150KB.
>
> ESP32 memory size: about 300KB

One possible solution is to separate the image binary into several patches and only allocate a relative smaller buffer in the memory that is capable to hold the largest patch. The patches are encoded into Base64 and sent out individually and concatenated together on the receiver (a more powerful machine).

There could be one issue caused by Base64 padding: if the length of the patches (in bytes) are not multiple times of 3, each patch will be padded at the end, and the binary will be incorrect after concatenation. To avoid the padding we must ensure that all the patches have the size of multiple times of 3 (except for the last patch). Otherwise we must decode each patch, discard the end zeros and encode these patches together, which causes additional computation cost.






# Adalight quickstart guide

# hardware I use:
- controller:
 - [wemos d1 mini](https://www.wemos.cc/en/latest/d1/d1_mini.html)
 - [Acustomarcade PICOCRT](https://acustomarcade.com/product/ws2812b-rgb-adapter-30-rgb)
- leds:
 - WS2812 led string, [for example](https://www.aliexpress.com/item/32243084800.html)
 - WS2812B 5V LED Strip Lights [like](https://www.amazon.com/gp/product/B0CFTP3S41)


## instructions for the d1 mini or simillar:
First step is to follow the instructions at [WLED](https://github.com/Aircoookie/WLED)
you are then left with a wifi + usb controlled led strip.

What I do next is create a preset with leds ON but black and set it as the wled boot preset.
When I'm ok with the settings (RGB order and max brightness mostly) and the strip is properly tested I disable the wifi (it forces you to reflash wled if you have to change something)

## Adalight protocol

https://www.partsnotincluded.com/visualizing-adalight-header-information/
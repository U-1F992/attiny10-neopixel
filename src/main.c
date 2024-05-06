#include <util/delay.h>

#include <stddef.h>

#include <ws2812_config.h>
#include <light_ws2812.h>

struct cHSV
{
    uint8_t h;
    uint8_t s;
    uint8_t v;
};

#define HUE_DEVISOR (43) /* Approx. ((double)UINT8_MAX / 6) */

void hsv_to_rgb(struct cHSV *hsv, struct cRGB *rgb)
{
    uint8_t region = hsv->h / HUE_DEVISOR;
    uint8_t remainder = (hsv->h % HUE_DEVISOR) * 6;

    uint8_t p = (hsv->v * (UINT8_MAX - hsv->s)) / UINT8_MAX;
    uint8_t q = (hsv->v * (UINT8_MAX - ((hsv->s * remainder) / UINT8_MAX))) / UINT8_MAX;
    uint8_t t = (hsv->v * (UINT8_MAX - ((hsv->s * (UINT8_MAX - remainder)) / UINT8_MAX))) / UINT8_MAX;

    switch (region)
    {
    case 0:
        rgb->r = hsv->v;
        rgb->g = t;
        rgb->b = p;
        return;
    case 1:
        rgb->r = q;
        rgb->g = hsv->v;
        rgb->b = p;
        return;
    case 2:
        rgb->r = p;
        rgb->g = hsv->v;
        rgb->b = t;
        return;
    case 3:
        rgb->r = p;
        rgb->g = q;
        rgb->b = hsv->v;
        return;
    case 4:
        rgb->r = t;
        rgb->g = p;
        rgb->b = hsv->v;
        return;
    case 5:
    default:
        rgb->r = hsv->v;
        rgb->g = p;
        rgb->b = q;
        return;
    }
}

#define LEDS ((size_t)2)
static struct cHSV hsv[LEDS] = {{.h = 0, .s = UINT8_MAX, .v = UINT8_MAX},
                                {.h = 127, .s = UINT8_MAX, .v = UINT8_MAX}};
static struct cRGB rgb[LEDS];
#define HUE_STEP ((uint8_t)8)

int main(void)
{
#ifdef __AVR_ATtiny10__
    CCP = 0xD8; // configuration change protection, write signature
    CLKPSR = 0; // set cpu clock prescaler =1 (8Mhz) (attiny 4/5/9/10)
#endif

    while (1)
    {
        hsv_to_rgb(&hsv[0], &rgb[0]);
        hsv_to_rgb(&hsv[1], &rgb[1]);
        ws2812_setleds(rgb, LEDS);
        _delay_ms(50);

        hsv[0].h += HUE_STEP;
        hsv[1].h += HUE_STEP;
    }
}
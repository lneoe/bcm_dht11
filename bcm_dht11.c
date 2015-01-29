#include <stdio.h>
#include <bcm2835.h>
#include <sys/time.h>

#define DHT11 11
#define MAXTIME 100


int readDHT(int type, int pin, float *hum_p, float *temp_p) {
    int counter = 0;
    int lastState = HIGH;
    int i, j = 0;
    int data[5] = {0, 0, 0, 0, 0};

    if (!bcm2835_init())
        return 0;

    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
    // bcm2835_gpio_write(pin, HIGH);
    // delayMicroseconds(50000); // 50 ms
    bcm2835_gpio_write(pin, LOW);
    delayMicroseconds(50000); // 50 ms
    bcm2835_gpio_write(pin, HIGH);
    delayMicroseconds(40);

    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);

    while(bcm2835_gpio_lev(pin) == 1) {
        delayMicroseconds(1);
    }

    for (i = 0; i < MAXTIME; i++) {
        counter = 0;

        while (bcm2835_gpio_lev(pin) == lastState) {
            counter++;
            // delayMicroseconds(1);
            if (counter == 1000) // 500 is ok?
                break;
        }
        lastState = bcm2835_gpio_lev(pin);

        if (counter == 1000) // 500 is ok?
            break;

        if ((i >= 4) && (i % 2 == 0)) {
            data[j/8] <<= 1;
            if (counter > 200)
                data[j/8] |= 1;
            j++;
        }
    }

    if ((j >= 40) && (data[4] == (data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        #ifdef DEBUG
        printf("Hum: %d\tTemp:%d\n", data[0], data[2] );
        printf("%d %d %d %d %d\n", data[0], data[1], data[2], data[3], data[4]);
        #endif

        *hum_p = data[0];
        *temp_p = data[2];

        return 1;
    }
    else {
        #ifdef DEBUG
        printf("some err\n");
        printf("%d %d %d %d %d\n", data[0], data[1], data[2], data[3], data[4]);
        #endif
        return 0;
    }

    #ifdef DEBUG
    printf("%d\n", j);
    printf("%d %d %d %d %d\n", data[0], data[1], data[2], data[3], data[4]);
    return 0;
    #endif
}


int main(void){
    int rtn;
    float temp_p, hum_p = .0;
    rtn = readDHT(11, 4, &hum_p, &temp_p);
    if (rtn) {
        printf("RH: %.2f \tTEMP: %.2f\n", hum_p, temp_p);
    }
}

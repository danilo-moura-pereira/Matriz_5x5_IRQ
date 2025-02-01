#include <stdio.h>
#include "pico/stdlib.h"
#include "lib/animacao/animacao.h"

#define GPIO_BOTAO_A 5
#define GPIO_BOTAO_B 6

// Função que configura PIO da Matriz de LEDs
uint pio_config(PIO pio) {
    bool ok;

    //coloca a frequência de clock para 128 MHz, facilitando a divisão pelo clock
    ok = set_sys_clock_khz(128000, false);

    printf("iniciando a transmissão PIO");
    if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys));

    //configurações da PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    return sm;
}

// Rotina principal
int main() {

    PIO pio = pio0; 
    uint16_t i;
    uint sm = pio_config(pio);

    while (true) {
        char tecla = detect_button(); // Armazena a tecla detectada
        printf("Tecla pressionada: %c\n", tecla);
        switch (tecla) {
            case 'A':
                led_matrix_main_animation(tecla, pio, sm);
                break;
            case 'B':
                ligar_leds_verdes(pio, sm);
                break;
        }
        sleep_ms(100); // Delay para evitar múltiplas leituras rápidas
    }
}

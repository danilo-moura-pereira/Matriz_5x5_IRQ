#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "lib/animacao/animacao.h"
#include "ws2812.pio.h"

#define GPIO_BOTAO_A    5
#define GPIO_BOTAO_B    6
#define GPIO_LED_R      13
#define GPIO_LED_G      11
#define GPIO_LED_B      12
#define GPIO_WS2812     7
#define NUM_LEDS        25

/*
 * Cria variáveis globais
 * static: variável permanece na memória durante toda a execução do programa 
 * volatile: a variável pode ser alterada por eventos externos
 */ 
static volatile uint contador = 0; // Contador para exibir o número correto na Matriz de LEDs 5x5
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)
static volatile PIO pio;
static volatile uint sm = 0;

// Inicializa vetor para apagar todos os leds
static double animacao_apaga[NUM_LEDS] = {0.0, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0.0, 
                                          0.0, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0.0};

// Prototipação da função de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events) {
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica se passou tempo suficiente desde o último evento (200 ms de debouncing)
    if (current_time - last_time > 200000) {
        last_time = current_time; // Atualiza o tempo do último evento

        // Exibe número na Matriz de LEDs 5x5
        printf("Mudanca do número exibido na Matriz de LEDs 5x5. Numero exibido= %d\n", contador);
        if (gpio == GPIO_BOTAO_A) {
            if (contador == 9) {
                contador = -1;
            }
            contador++; // Incrementa contador
            gera_animacao(contador, pio, sm);
        } 
        else if (gpio == GPIO_BOTAO_B) {
            if (contador == 0) {
                contador = 10;
            }
            contador--; // Decrementa contador
            gera_animacao(contador, pio, sm);
        }
    }
}

// Função que configura PIO da Matriz de LEDs 5x5
uint pio_config(PIO pio) {
    bool activateClock;
    activateClock = set_sys_clock_khz(128000, false);

    // Inicializa PIO
    uint offset = pio_add_program(pio, &ws2812_program);
    uint sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm, offset, GPIO_WS2812);

    return sm;
}

// Inicializa LEDs RGB
void init_leds() {
    gpio_init(GPIO_LED_R);
    gpio_set_dir(GPIO_LED_R, GPIO_OUT);

    gpio_init(GPIO_LED_G);
    gpio_set_dir(GPIO_LED_G, GPIO_OUT);

    gpio_init(GPIO_LED_B);
    gpio_set_dir(GPIO_LED_B, GPIO_OUT);
}

// Inicializa botões A e B
void init_botoes() {
    gpio_init(GPIO_BOTAO_A);
    gpio_set_dir(GPIO_BOTAO_A, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(GPIO_BOTAO_A); // Habilita o pull-up interno

    gpio_init(GPIO_BOTAO_B);
    gpio_set_dir(GPIO_BOTAO_B, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(GPIO_BOTAO_B); // Habilita o pull-up interno
}

// Rotina principal
int main() {
    stdio_init_all();

    init_leds();
    init_botoes();

    printf("Matrix de LEDs 5x5 - Animacoes com numeros de 0-9\n");

    pio = pio0; 
    sm = pio_config(pio);

    // Apaga LEDs
    animacao_pio(animacao_apaga, 0, pio, sm, 0, 0, 0);

    // Configuração da interrupção com callback ao acionar os botões A ou B
    gpio_set_irq_enabled_with_callback(GPIO_BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(GPIO_BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true) {
        // Pisca LED vermelho 5 vezes por segundo
        gpio_put(GPIO_LED_R, 1);
        sleep_ms(12); 
        gpio_put(GPIO_LED_R, 0);
        sleep_ms(12); 
    }
}
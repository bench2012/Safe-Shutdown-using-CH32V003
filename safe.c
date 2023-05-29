#define SYSTEM_CORE_CLOCK 48000000
#define SYSTICK_USE_HCLK
#define APB_CLOCK SYSTEM_CORE_CLOCK

#include "ch32v003fun.h"
#include "wiring.h"
#include <stdio.h>


#define BUTTON_PIN  pin_C5  // GPIO pin for the button input
#define RELAY_PIN   pin_C6  // GPIO pin for the relay output
#define SHUTDOWN_PIN    pin_C7  // GPIO pin for the shutdown output
#define STATUS_PIN  pin_D0  // GPIO pin for the status input
#define SHUTDOWN_TIMER  30000  // Shutdown timer in millseconds
#define HOLD_TIME 2000	// Button hold time in millseconds
#define HIGH 1 // Define state High
#define LOW 0 // Define state Low

/* some bit definitions for systick regs */
#define SYSTICK_SR_CNTIF (1<<0)
#define SYSTICK_CTLR_STE (1<<0)
#define SYSTICK_CTLR_STIE (1<<1)
#define SYSTICK_CTLR_STCLK (1<<2)
#define SYSTICK_CTLR_STRE (1<<3)
#define SYSTICK_CTLR_SWIE (1<<31)

volatile uint32_t systick_cnt;

/*
 * Start up the SysTick IRQ
 */
void systick_init(void)
{
	/* disable default SysTick behavior */
	SysTick->CTLR = 0;
	
	/* enable the SysTick IRQ */
	NVIC_EnableIRQ(SysTicK_IRQn);
	
	/* Set the tick interval to 1ms for normal op */
	SysTick->CMP = (SYSTEM_CORE_CLOCK/1000)-1;
	
	/* Start at zero */
	SysTick->CNT = 0;
	systick_cnt = 0;
	
	/* Enable SysTick counter, IRQ, HCLK/1 */
	SysTick->CTLR = SYSTICK_CTLR_STE | SYSTICK_CTLR_STIE |
					SYSTICK_CTLR_STCLK;
}

/*
 * SysTick ISR just counts ticks
 * note - the __attribute__((interrupt)) syntax is crucial!
 */
void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void)
{
	// move the compare further ahead in time.
	// as a warning, if more than this length of time
	// passes before triggering, you may miss your
	// interrupt.
	SysTick->CMP += (SYSTEM_CORE_CLOCK/1000);

	/* clear IRQ */
	SysTick->SR = 0;

	/* update counter */
	systick_cnt++;
}



int main(void)
{
    SystemInit48HSI();
//	SetupDebugprintf();
	// Enable GPIO ports
	portEnable(port_C);
	portEnable(port_D);

    int relay_state = 0;    // Relay is initially off
    int shutdown_state = 0; // Shutdown is initially off
    int status_state = 0;   // Status is initially low
    int button_state = 0;   // Button is initially not pressed
    int button_last_state = 0;  // Last state of the button
    int button_press_time = 0;  // Time the button was pressed
    int shutdown_ticks = 0; //Shutdown Ticks

    // Setup GPIO pins as inputs or outputs
    
    pinMode(BUTTON_PIN, pinMode_I_pullDown);
	pinMode(RELAY_PIN, pinMode_O_pushPull);
	pinMode(SHUTDOWN_PIN, pinMode_O_pushPull);
	pinMode(STATUS_PIN, pinMode_I_pullDown);
	digitalWrite(RELAY_PIN, 0);
	digitalWrite(SHUTDOWN_PIN, 0);
	//Initialize system tick
	systick_init();


    // Main loop
    while (1) {
        // Read the button state
        button_last_state = button_state;
        button_state = digitalRead(BUTTON_PIN);
     
        
//        while (relay_state == 1 && shutdown_state == 0 )
        while (relay_state == 1 ) 
        {
        // Check for long and short press
        button_last_state = button_state;
	    button_state = digitalRead(BUTTON_PIN);
        if ( button_state == HIGH && button_last_state == LOW ) {
            button_press_time = systick_cnt;
           // printf( "Button pressed shutdown detect\n\r");
           // printf( "button_press_time: %d\n\r", button_press_time);         
                                                                }
        
         // Check for short press (<2 sec) if the button change state from High to Low
        if ( button_state == LOW && button_last_state == HIGH ) {
                // Release: trigger shutdown state
                shutdown_state = 1;
                digitalWrite(SHUTDOWN_PIN, 1);
        	if (systick_cnt - button_press_time > HOLD_TIME) {
           										
               	printf( "button_ticks: %lu\n\r", (systick_cnt - button_press_time));								    
                // Long press: initiate shutdown sequence and power off sqeuence
                shutdown_ticks = systick_cnt;
               // printf( "Long press: initiate shutdown sequence and power off sqeuence\n\r");
                // Check for shutdown timer expiration and system status pin
                // Read the status input
                status_state = digitalRead(STATUS_PIN);
        		while (systick_cnt - shutdown_ticks < SHUTDOWN_TIMER && status_state == 0 ) {
        		    status_state = digitalRead(STATUS_PIN);
        		   // printf( "System Status: %d / Shutdown Timer Milliseconds: %lu\n\r", status_state, ((systick_cnt - shutdown_ticks)));
				                                                            							}
                relay_state = 0;
            	digitalWrite(RELAY_PIN, 0);
             }
                
         														} 
         }
         	   // printf( "1 Button current state:  %d\n\r", button_state);
        		printf( "2 Button pressed. Last state:  %d\n\r", button_last_state);
        		button_last_state = button_state;
                button_state = digitalRead(BUTTON_PIN);
         // Cheak if relay is off ie. power down 
		while (relay_state == 0 ) { 
		// Do nothing if button is not press
	       while (button_state == LOW && button_last_state == LOW) {
	            button_last_state = button_state;
			  	button_state = digitalRead(BUTTON_PIN);
			  	printf( "Start loop Button current state:  %d\n\r", button_state);
        		printf( "Start loop Button Last state:  %d\n\r", button_last_state);
			  	Delay_Ms(50);
                 			  		  	                           }
        // Turn on power          			  		  	                           

			 // printf( "Button pressed startup\n\r");
       // Wait for button press
		   while (button_state == HIGH && button_last_state == LOW ) {
			  	button_last_state = button_state;
			  	button_state = digitalRead(BUTTON_PIN);
			  	printf( "Button pressed. Button current state:  %d\n\r", button_state);
        		printf( "Button pressed. Last state:  %d\n\r", button_last_state);
			  	Delay_Ms(50);
    		  	                                                     }
			  relay_state = 1;
			  shutdown_state = 0;
			  digitalWrite(SHUTDOWN_PIN, 0);
			  digitalWrite(RELAY_PIN, 1);  
               					  
           while (button_state == HIGH && button_last_state == HIGH ) {
                button_last_state = button_state;
			  	button_state = digitalRead(BUTTON_PIN);
			  	printf( "Wait for release. Button current state:  %d\n\r", button_state);
        		printf( "Wait for release. Last state:  %d\n\r", button_last_state);
			  	Delay_Ms(50);
				                                                      }
				                  }								            		

		}
return(0);
}

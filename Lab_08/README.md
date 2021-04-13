##  Lab Report ##

Your Name: 
-----------


Lab Status
-------
Answer the following questions by put a "x" in the bracket.
- Yes [], No []: Complete pre-lab
- Yes [], No []: Complete basic lab requirements
- Yes [], No []: Complete something cool
  - If you have completed something cool, give it a short summary below: 


Answers to Pre-lab Questions
-------
This part is required only if you are requested to write your answers here. 

* Question 1:
RCC_AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

* Qestion 2:
GPIOA->MODER &= ~(3UL<<(2*LED_PIN));  
GPIOA->MODER &= ~(3UL);
GPIOA->MODER |= 2UL<<(2*LED_PIN);      // AlternateFunction(10)
GPIOA->MODER |= 2UL;
GPIOC->MODER &= ~(3UL<<(2*BUTTON_PIN));

* Question 3:
GPIOA->AFR[0] &= ~(0xF); 
GPIOA->AFR[0] |= 1UL << 20; //set PA5 to AF1 for TIM2 CH1
GPIOA->AFR[0] &= ~(0xF); 
GPIOA->AFR[0] |= 2UL; //set PA0 to AF2 for TIM5 CH1

* Question 4: 

* Question 5: 
-90 -> 1750
0 	-> 1870
90 	-> 1950

Answers to After-lab Questions
-------

* Question 1:
PSC -> 4Million (if running at default speed);
CRR -> 2Million
ARR -> ?

* Question 2:
big

Suggestions and Comments
-------

* Do you have comments or suggestions for the lab and course?


* Do you have comments or suggestions for the textbook and lab handouts? Any typos or errors?



Lab Credits
-------
Did you received any help from someone other than the instructor and lab teaching assistants?

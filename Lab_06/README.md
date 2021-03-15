##  Lab Report ##

Your Name: 
-----------


Lab Status
-------
Answer the following questions by put a "x" in the bracket.
- Yes [x], No []: Complete pre-lab
- Yes [x], No []: Complete basic lab requirements
- Yes [x], No []: Complete something cool
  - If you have completed something cool, give it a short summary below: 
I wrote a simple PWM program. it's not that cool but I got to pulse the light instead of just turning on an off (even though it is just turning on and off really fast).

Answers to Pre-lab Questions
-------
This part is required only if you are requested to write your answers here. 

* Question 1:
Interrupts are more efficient and use less power. polling is much more dedicated

* Question 2:
	NVIC_EnableIRQ(EXTI15_10_IRQn);

* Question 3:
	void EXTI15_10_IRQHandler(void);

* Question 4:
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3] |= ~SYSCFG_EXTICR4_EXTI13_PC;
* Question 5:
	EXTI->RTSR1 |=  EXTI_RTSR1_RT13;
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT13;
	EXTI->IMR1 	|=  EXTI_IMR1_IM13;
Answers to After-lab Questions
-------

* Question 1:
You have to write 1 to it (or set it).
EXTI->PR1 |= EXTI_PR1_PIF13; 

* Question 2:
If you dont clear the bit the interrupt will always be active so it won't be able to clear out of it.

* Question 3:

* Question 4:
Low speed is good for LEDs because first of all, the human eye can't tell the difference between a 10mHz led and an 80mHz, so why waste energy on keeping an
LED lit if we can't see the difference between them? 
Suggestions and Comments
-------

* Do you have comments or suggestions for the lab and course?


* Do you have comments or suggestions for the textbook and lab handouts? Any typos or errors?



Lab Credits
-------
Did you received any help from someone other than the instructor and lab teaching assistants?

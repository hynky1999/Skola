.dseg;
.org 0x100

sekundy: .byte 2;
joystick: .byte 1;
print_pos: .byte 1;
num_to_print: .byte 1;
add_time: .byte 1;
timer_flag: .byte 1;


.cseg;
.include "m169def.inc";

.org 0x1000
.include "print.inc";

.org 0
jmp start
.org 0xA
jmp interrupt

.org 0x100;
retezec: .db "BUM BUM VYBOUCHLO TO !! ", 0;
delka: .db 5; Delka vice nez 5 je problematicka znak by na hodinach mohl zustat -> lze vyresit promazanim hodim pri initu

start:
	 ; Inicializace zasobniku
    ldi r16, 0xFF;
    out SPL, r16;
    ldi r16, 0x04;
    out SPH, r16;

    ; Inicializace displeje
    call init_disp;
    ; Inicializace preruseni od casovace
    call init_int;
	


pre_set_time: 
	ldi r16, 0;
	sts sekundy, r16;
	sts sekundy+1, r16;	Nastav cas na 0;
	ldi r18, 60;
	sts add_time, r18;	Nastav pridavani na minuty

set_time:
	call show_clk;
	lds r17, joystick; Nacti joysticky
	call read_joy;
	lds r16, joystick;
	andi r16, 0b11010000;
	andi r17, 0b11010000;
	cp r16, r17;	Pokud je klavesa stejna jako minule necti ji
	breq set_time;
	lds r16, joystick;
	mov r17, r16;
	andi r17,0b10000000;	dolu
	brne up_joy;
		call dec_num;
		jmp set_time;
		  
up_joy:
	mov r17, r16;
	andi r17, 0b01000000; nahoru
	brne enter_joy;
		call inc_num;
		jmp set_time;
		 
enter_joy:
	mov r17, r16
	andi r17, 0b00010000; enter
	brne set_time;
		lds r18, add_time;
		cpi r18, 60;	
		brne pre_timer_loop;
			ldi r18, 1; Zmen minuty na sekundy
			sts add_time, r18;
			jmp set_time;

pre_timer_loop:
	ldi r16, 0; Nastav flag aby se hned nezapocitala sekunda
	sts timer_flag, r16;

timer_loop:
	lds r16, timer_flag;
	tst r16;
	breq timer_no_change;
		call dec_num; Pokud ubehla sekunda updatuju timer
		call show_clk;
		ldi r16, 0;
		sts timer_flag, r16;

timer_no_change:
	lds r16, sekundy;
	tst r16;
	brne timer_loop;
	lds r16, sekundy+1; Podivej se jestli se cas == 0
	tst r16;
	brne timer_loop;

	ldi r16, 9; 4-> bliky
timer_blik:
	lds r17, timer_flag;
	tst r17;
	breq timer_blik;
	ldi r17, 0;
	sts timer_flag, r17; Zmen kazdou sekundu stav -> pulblik

	dec r16;
	breq timer_end;
	ldi r17, 2;
	sts print_pos, r17;
	mov r17, r16;
	ror r17;
	brcc timer_blik_blank; Pri lichem cisle ukaz hodiny
		call show_clk;
		jmp timer_blik;

timer_blik_blank:
	ldi r17, ' '; Pri sudem cisle neukazuj nic
	sts num_to_print, r17;
	call show_as_char;
	call show_as_char;
	ldi r17, ':';
	sts num_to_print, r17;
	call show_as_char;
	ldi r17, ' ';
	sts num_to_print, r17;
	call show_as_char;
	call show_as_char;
	jmp  timer_blik;

	
timer_end:
	; Vicemene kod z meho 8 ukolu
	ldi r30, low(2*delka); Nacti delku 
	ldi r31, high(2*delka);
	lpm r18, Z;
	inc r18; Nastav delka + 2 abychom mohli cekovat proti dipleji
	inc r18;
	ldi r30, low(2*retezec);
	ldi r31, high(2*retezec); Nastav ukazatel na prvni pismeno

infinite_cycle:
	ldi r17, 2; Nastav prvni pozici displeje
	push r30; 
	push r31; Uloz puvod ukazatel
print_hlaska:
	call get_next_char; Nacti dalsi char do r16
	call show_char;
	inc r17; 
	cp r17, r18; 
	brne print_hlaska;

	pop r31; Nacti puvod ukazatel a posnu o jedna
	pop r30;
	call get_next_char;
wait_cycle_hlaska:
	call read_joy;
	lds r16, joystick;
	andi r16, 0b00010000; Cekej dokud nedostanes enter nebo uplyne 1 sekunda
	brne PC+2;
		jmp pre_set_time;
	lds r16, timer_flag;
	tst r16;
	breq wait_cycle_hlaska;
	ldi r16, 0;
	sts timer_flag, r16;
	jmp infinite_cycle;
	


get_next_char:
	lpm r16, Z+; Nacti char
	tst r16;
	breq PC+2; Pokud neni nula vrat
		ret;
	ldi r30, low(2*retezec); Jinak nastav char na zacatek
	ldi r31, high(2*retezec);
	lpm r16, Z+;
	ret; 

inc_num:
	push r17; Pridej add_time k sekundam
	push r16;
	lds r16, sekundy+1; 
	lds r17, sekundy;
	lds r18, add_time;
	add r16, r18;
	ldi r18, 0;
	adc r17, r18;
	sts sekundy+1, r16;
	sts sekundy, r17;
	pop r16;
	pop r17;
	ret;

dec_num:
	push r18; Odeber add_time od sekund
	push r17;
	push r16;
	lds r16, sekundy+1;
	lds r17, sekundy;
	tst r16;
	brne PC+3;
		tst r17;
		breq dec_num_end; Pokud je nula neodebirej nic
	lds r18, add_time;
	neg r18;
	add r16, r18;
	ldi r18, 255;
	adc r17, r18;
	sts sekundy+1, r16;
	sts sekundy, r17;
dec_num_end:
	pop r16;
	pop r17;
	pop r18;
	ret;

show_clk:
	push r18;
	push r17;
	push r16;

	ldi r18, 2;
	sts print_pos, r18;
	lds r16, sekundy;
	lds r17, sekundy+1;
	clr r18;
	for_minuty:
		tst r16;
		brne PC+3;	Odecitej 60 dokud muzes pri kazdem odecten zvys r17
			cpi r17, 60;
			brlo for_minuty_end;
		subi r17, 60;
		sbci r16, 0;
		inc r18;
		jmp for_minuty;
		
for_minuty_end:
	sts num_to_print, r18; Vypis minuty
	call show_in_dec;
	ldi r18, ':';
	sts num_to_print, r18;
	call show_as_char;
	sts num_to_print, r17; Vypis sekundy
	call show_in_dec;
	pop r16;
	pop r17;
	pop r18
	ret;

show_as_char:
	push r17; Vypis jak character z num_to_print na pozici print_pos
	push r16;
	in r16, SREG;
	push r16;
	lds r16, num_to_print;
	lds r17, print_pos;
	call show_char;
	inc r17;
	sts print_pos, r17;
	pop r16;
	out SREG, r16;
	pop r16;
	pop r17;
	ret;

show_in_dec:
	push r18;
	push r17;
	push r16;
	in r16, SREG;
	push r16;
	lds r18, num_to_print;
	lds r17, print_pos;
	ldi r16, 48;
loop_dec:
	cpi r18, 10; Odecitej 10 dokud muzes a pri kazdem pricteni zvys r16 -> v r16 budou desitky
	brlo print_res;
	inc r16;
	subi r18, 10;
	jmp loop_dec;

print_res:
	call show_char; Vypis desitky
	inc r17;
	ldi r16, 48;
	add r16, r18;
	call show_char; Vypis jednotky
	inc r17;
	sts print_pos, r17;
	pop r16;
	out SREG, r16;
	pop r16;
	pop r17;
	pop r18;
	ret;
		
init_int:
    cli              ; globalni zakazani preruseni
    ldi r16, 0b00001000
    sts ASSR, r16    ; vyber hodin od externiho krystaloveho oscilátoru 32768 Hz
    ldi r16, 0b00000001
    sts TIMSK2, r16  ; povoleni preruseni od casovace 2
    ldi r16, 0b00000101
    sts TCCR2A, r16  ; nastaveni deliciho pomeru 128
    clr r16
    out EIMSK, r16   ; zakazani preruseni od joysticku
    sei              ; globalni povoleni preruseni
	ret

interrupt:
	push r16;
	in r16, SREG;
	push r16;
	ldi r16, 1;
	sts timer_flag, r16;
	pop r16;
	out SREG, r16;
	pop r16;
	reti;

init_joy:                ; inicializace joysticku
    in r17, DDRE
    andi r17, 0b11110011
    in r16, PORTE
    ori r16, 0b00001100
    out DDRE, r17
    out PORTE, r16
    ldi r16, 0b00000000
    sts DIDR1, r16
    in r17, DDRB
    andi r17, 0b00101111
    in r16, PORTB
    ori r16, 0b11010000
    out DDRB, r17
    out PORTB, r16
	ret

read_joy:                ; ulozi smer joysticku do registru r20
    push r18             ; uklid r16 a r17
    push r17
	push r16

joy_reread:
    in r16, PINB         ; nacti hodnotu joysticku 1
	in r17, PINE
	or r16, r17;

    ldi r20, 255         ; chvili cekej 2
joy_wait: dec r20
    brne joy_wait

    in r17, PINB         ; nacti jeste jednou
	in r18, PINE;
	or r17, r18;

    cp r16, r17
    brne joy_reread      ; hodnoty se nerovnaly -> nacti znovu
	sts joystick, r17;
joy_no_enter:

	pop r16              ; obnoveni r16 a r17
  	pop r17
	pop r18
	ret;

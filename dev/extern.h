// External custom code to be run from a script

// =======[CUSTOM MODIFICATION]=======
unsigned char *textbuff = 23458;
unsigned char *extaddress;
unsigned char exti, extx, exty, stepbystep, keyp;
unsigned char is_cutscene = 0;
//

void do_extern_action (unsigned char n) {
	// Add custom code here.
	
	if (n == 0) {
		// Cortina
		saca_a_todo_el_mundo_de_aqui (0);
		for (exti = 0; exti < 10; exti ++) {
			for (extx = exti; extx < 30 - exti; extx ++) {
				sp_PrintAtInv (VIEWPORT_Y + exti, VIEWPORT_X + extx, 71, 0);
				sp_PrintAtInv (VIEWPORT_Y + 19 - exti, VIEWPORT_X + extx, 71, 0);
				if (extx < 19 - exti) {
					sp_PrintAtInv (VIEWPORT_Y + extx, VIEWPORT_X + exti, 71, 0);
					sp_PrintAtInv (VIEWPORT_Y + extx, VIEWPORT_X + 29 - exti, 71, 0);
				}
			}
			#asm
				halt
			#endasm
			sp_UpdateNow ();
		}
		return;
	}
	
	// Show text n
	stepbystep = 1;
	
	//asm_number [0] = (n - 1) << 1;
	asm_int [0] = (n - 1) << 1;
#asm
		di
		ld b, 6
		call SetRAMBank
	
		; First we get where to look for the packed string
		
		; esto ya no me vale
		;ld d, 0
		;ld a, (_asm_number)
		;ld e, a
		; necesito leer 16 bits...
		
		ld a, (_asm_int)
		ld e, a
		ld a, (_asm_int + 1)
		ld d, a
		
		ld hl, $c000
		add hl, de
		ld c, (hl)
		inc hl
		ld b, (hl)
		push bc
		pop hl
		
		ld de, 23458

		; 5-bit scaped depacker by na_th_an
		; Contains code by Antonio Villena
		
		ld a, $80

.fbsd_mainb
        call fbsd_unpackc
        ld c, a
        ld a, b
        and a
        jr z, fbsd_fin
        call fbsd_stor
        ld a, c
        jr fbsd_mainb	

.fbsd_stor
        cp 31
        jr z, fbsd_escaped
        add a, 64
        jr fbsd_stor2
.fbsd_escaped
        ld a, c
        call fbsd_unpackc
        ld c, a
        ld a, b
        add a, 32
.fbsd_stor2
        ld (de), a
        inc de
        ret

.fbsd_unpackc
        ld      b, 0x08
.fbsd_bucle
        call    fbsd_getbit
        rl      b
        jr      nc, fbsd_bucle
        ret

.fbsd_getbit
        add     a, a
        ret     nz
        ld      a, (hl)
        inc     hl
        rla
        ret        
        
.fbsd_fin
		ld (de), a	
		;
		;		
		
		ld b, 0
		call SetRAMBank
		ei
#endasm	
	
	if (is_cutscene == 0) {
		// Show
		exti = textbuff [0] - 64;
		
		// Draw empty frame
		extx = 3 + exti + exti;
		exti = !((VIEWPORT_Y + (p_y >> 9)) < extx);	
		saca_a_todo_el_mundo_de_aqui (exti);
	
		print_str (3, 3, 1, "#$$$$$$$$$$$$$$$$$$$$$$$$%");
		for (exty = 4; exty < extx; exty ++) print_str (3, exty, 1, "&                        '");
		print_str (3, extx, 1, "())))))))))))))))))))))))*");
		
		exty = 4;
	} else {
		exty = 13;
	}
	
	// Draw text
	extx = 4; 
	extaddress = textbuff + 1;
	keyp = 1;
	while (exti = *extaddress ++) {
		if (exti == '%') {
			extx = 4; exty += 2;
		} else {
			sp_PrintAtInv (exty, extx, 71, exti - 32);
			extx ++;
			if (extx == 28) {
				extx = 4; exty += 2;
			}
		}
		if (stepbystep) {
#asm
			halt
#endasm
			if (exti != 32 && is_cutscene == 0) wyz_play_sound (8);
#asm
			halt
			halt
#endasm
			sp_UpdateNow ();
		}
		
		//if (sp_GetKey () != 0) {
		if (button_pressed ()) {
			if (keyp == 0) {
				stepbystep = 0;
			} 
		} else {
			keyp = 0;
		}
	}
	sp_UpdateNow ();
	/*
	sp_WaitForNoKey ();
	while (sp_GetKey () == 0);*/
	sp_WaitForNoKey ();
	while (button_pressed ());
	espera_activa (5000);
	
	return;
}

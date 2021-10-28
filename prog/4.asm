.386p

descr struc
    limit   dw 0
    base_l  dw 0
    base_m  db 0
    attr_1  db 0
    attr_2  db 0
    base_h  db 0
descr ends

idescr struc
    offs_l  dw 0
    sel     dw 0
    cntr    db 0
    attr    db 0
    offs_h  dw 0
idescr ends

stk segment  para stack 'STACK'
    stack_start db  100h dup(?)
    stack_size = $-stack_start
stk ends

data segment para 'data'
    gdt_null  descr <>
    gdt_data4gb descr <0FFFFh,0,0,92h,0CFh> ; attr1: p=1 dpl = 00 1 тип: 0010(чтение и запись); attr2: g=1 d=1 0 avl=0 граница:1111
    gdt_code32 descr <code2_size-1,0,0,98h,40h>  ; attr1: p=1 dpl = 00 1 тип: 1000(исполнение); atr2 = d = 1 000000
    gdt_data descr <data_size-1,0,0,92h,40h> ; attr1: p=1 dpl = 00 1 тип: 0010(чтение и запись); atr2 = d = 1 000000
    gdt_stack32 descr <stack_size-1,0,0,92h,40h> ; attr1: p=1 dpl = 00 1 тип: 0010(чтение и запись); atr2 = d = 1 000000
    gdt_video16 descr <3999,8000h,0Bh,92h>

    gdt_size=$-gdt_null
    pdescr    df 0

    ;codes=8
	;data4gbs=16
    ;code2s=24
    ;datas=32
    ;stks=40
    ;screens=48
    data4gbs=8
    code2s=16
    datas=24
    stks=32
    screens=40

    idt label byte
	idescr_0_12 idescr 13 dup (<dummy,code2s,0,8Fh,0>)
    idescr_13 idescr <except_13,code2s,0,8Fh,0>
    idescr_14_31 idescr 18 dup (<dummy,code2s,0,8Fh,0>)
    int08 idescr <timerInt,code2s,0,10001110b,0> ; p = 1 dpl = 00 0 тип = 1110 (прервания)
    int09 idescr <keybInt,code2s,0,10001110b,0>
    idt_size = $-idt 

    ipdescr df 0
    ipdescr16 dw 3FFh, 0, 0

    mask_master db 0        
    mask_slave  db 0        

    interval_time      db 0            

    interval=10
    enter_but db 0
    mempos=80*2+mem_msg_len*2+7*2
    cursor_pos=mempos+6
    syml_pos dw 80*2*2
    cursor_symb_on=220
    cursor_symb_off=223
    prmod_pos=0
    memmsg_pos=80*2

    protectmode  db  ' '
    protectmode_len=$-protectmode
    mem_msg db 'Memory: '
    mem_msg_len=$-mem_msg
    rm_msg      db 27, '[30;42mNow in Real Mode. ', 27, '[0m$', '$'
    pm_msg_out  db 27, '[30;42mNow in Real Mode again! ', 27, '[0m$'
    color=17h
	color_black=0h
	shift db 0

    asciimap db 0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0
    db 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0
    db 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 0, 0, 0, 92
    db 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'
	db 0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0
    db 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0
    db 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', 0, 0, 0, '|'
    db 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?'
    data_size = $-gdt_null 
data ends

code2 segment para public 'code' use32
     assume cs:code2, ds:data, ss:stk
     
    protected:
        mov ax, datas
        mov ds, ax
        mov ax, screens
        mov es, ax
        mov ax, stks
        mov ss, ax
        mov eax, stack_size
        mov esp, eax
		
		;Разрешение прерываний
		sti

        mov edi, prmod_pos
        mov ebx, offset protectmode
        mov ecx, protectmode_len
        mov ah, color

        loop_02:
            mov al, byte ptr [bx]
            inc bx
            stosw
        loop loop_02

        mov edi, memmsg_pos
        mov ebx, offset mem_msg
        mov ecx, mem_msg_len
        mov ah, color

        loop_03:
            mov al, byte ptr [bx]
            inc bx
            stosw
        loop loop_03

        call countMemory
        mov ax, datas
        mov ds, ax

        looplab:
        cmp enter_but, 1
        jne looplab
		
		;Запрет прерываний
		cli
        
        
		db  0EAh
        dd  offset next
        dw  code2s
	next:
        mov eax, cr0
        and al, 0FEh
        mov cr0, eax
        db  0EAh
        dd  offset real
		dw  code
    dummy proc
        iret
    dummy endp
	except_13 proc uses eax
        pop eax
        iret
    except_13 endp
    output_dec proc near
        push edx
        push ecx
        push ebx
        xor edx, edx
        mov ecx, 100000h
        div ecx
        mov ecx, 10
        mov bx, mempos
        prmem:
            xor edx, edx
            div ecx
            add dl, '0'
            mov dh, color
            mov es:[bx], dx
            dec bx
            dec bx
            cmp eax, 0
        jnz prmem
		add bx, 6
		mov ah, color
		mov al, 'M'
		mov es:[bx], ax
		add bx, 2
		mov al, 'B'
		mov es:[bx], ax

        pop ebx
        pop ecx
        pop edx
        ret
    output_dec endp

    countMemory proc 
        mov ax, data4gbs
        mov ds, ax

        push eax
        push ebx
        push edx
		push ecx
        mov ebx, 100000h
        mov eax, 0AEh
        xor ecx, ecx
    cloop:
		mov dh, [ebx]
        mov [ebx], eax
        cmp eax, [ebx]
        je cloop1
        jmp exit
    cloop1:
        inc ecx
    cloop2:
		mov [ebx], dh
        inc ebx
    
        jz exit
        jmp cloop
    exit:

    mov eax, ecx
	add eax, 100000h
    call output_dec

	pop ecx
    pop edx
    pop ebx
    pop eax
    ret
    countMemory endp

    timerInt proc uses eax
        cmp interval_time, interval
        je ON
        cmp interval_time, interval*2
        jne skip

        mov al, cursor_symb_off
        mov interval_time, 0
        jmp pr
    ON:
        mov al, cursor_symb_on
    pr:
        mov ah, color
        mov bx, cursor_pos
        mov es:[bx], ax      
    skip:
        mov  al, interval_time
        inc al
        mov interval_time, al

        mov al, 20h
        out 20h, al
        
        iretd
    timerInt endp

    keybInt proc ; Обработчик прерывания от клавиатуры
        push eax
        push bx
        push dx
        xor eax, eax
        in  al, 60h
        cmp al, 1Ch
        jne print
		or enter_but, 1
        jmp endlab
    print:
		cmp al, 0AAh
		je shift_off
        cmp al, 80h  ;  При отпускании клавиши контроллер клавиатуры посылает в тот же порт скан-код, увеличенный на 80h
        ja endlab
		cmp al, 0Eh
		je backspace
		cmp al, 2Ah
		je shift_on
		add al, shift
        mov dl, asciimap[eax]  
        mov al, dl
        mov ah, color
        mov bx, syml_pos
        mov es:[bx], ax      
        add syml_pos, 2
		jmp endlab
	backspace: 
        sub syml_pos, 2
		mov dl, ' '
        mov al, dl
        mov ah, color_black
        mov bx, syml_pos
        mov es:[bx], ax
		jmp endlab
	shift_on:
		mov shift, 54
		je endlab
	shift_off:
		mov shift, 0
    endlab:
        in  al, 61h 
        or  al, 80h 
        out 61h, al 
        and al, 7Fh 
        out 61h, al

        mov al, 20h 
        out 20h, al

        pop dx
        pop bx
        pop eax

        iretd
    keybInt endp
code2_size = $-protected
code2 ends

code segment para public 'CODE' use16
assume cs:code, ds:data, ss: stk
start:
        mov ax, data
        mov ds, ax

        ; очистить экран
        mov ax, 3
        int 10h

        mov ah, 09h
        lea dx, rm_msg
        int 21h
        xor dx, dx
        mov ah, 2
        mov dl, 13
        int 21h
        mov dl, 10
        int 21h

        ; ожидание ввода символа
        mov ah, 10h
        int 16h

        ; очистить экран
        mov ax, 3
        int 10h

        ; Загрузка линейного адреса сегмента команд в GDT
        xor eax, eax
        mov ax, code2
        shl eax, 4
        mov bx, offset gdt_code32
        mov (descr ptr [bx]).base_l, ax
        shr eax, 16
        mov (descr ptr [bx]).base_m, al
		
		        ; Загрузка линейного адреса сегмента данных в GDT
        xor eax, eax
        mov ax, data
        shl eax, 4
        mov bx, offset gdt_data
        mov (descr ptr [bx]).base_l, ax
        shr eax, 16
        mov (descr ptr [bx]).base_m, al

        ; Загрузка линейного адреса сегмента стека в GDT
        xor eax, eax
        mov ax, stk
        shl eax, 4
        mov bx, offset gdt_stack32
        mov (descr ptr [bx]).base_l, ax
        shr eax, 16
        mov (descr ptr [bx]).base_m, al

        ; загрузка gdtr через псевдодескриптор
        xor eax, eax
        mov ax, data
        shl eax, 4
        add eax, offset gdt_null
        mov dword ptr pdescr + 2, eax
        mov word ptr pdescr, gdt_size - 1
        lgdt fword ptr pdescr          

        ; сохранение масок  
        in  al, 21h                     
        mov mask_master, al             
        in  al, 0A1h                    
        mov mask_slave, al

        ; устанавливаем базовый вектор ведущего контроллера в 32
        mov al, 11h
        out 20h, al                     
        mov al, 32
        out 21h, al                     
        mov al, 4
        out 21h, al
        mov al, 1
        out 21h, al
        
        ; маска для ведущего контроллера
        mov al, 0FCh
        out 21h, al

        ; маска для ведомого контроллера
        mov al, 0FFh
        out 0A1h, al
		
		;Запрет прерываний
		cli
		mov al, 80h
		out 70h, al

        ; загрузка idtr через псевдодескриптор
        xor eax, eax
        mov ax, data
        shl eax, 4
        add eax, offset idt
        mov  dword ptr ipdescr + 2, eax 
        mov  word ptr  ipdescr, idt_size-1 
        lidt fword ptr ipdescr 
        
        ; открытие линии А20
        in  al, 92h
        or  al, 2
        out 92h, al

        ; Переход в защищенный режим
        mov eax, cr0
        or  eax, 1
        mov cr0, eax

        db  66h 
        db  0EAh
        dd  offset protected
        dw  code2s
        
    real:
        mov ax, data   
        mov ds, ax
        mov ax, stk   
        mov ss, ax
        mov ax, stack_size
        mov sp, ax

        ; восстановление ведущего контроллера, установив базовый вектор в 8
        mov al, 11h
        out 20h, al
        mov al, 8
        out 21h, al
        mov al, 4
        out 21h, al
        mov al, 1
        out 21h, al

        ;восстановление масок 
        mov al, mask_master
        out 21h, al
        mov al, mask_slave
        out 0A1h, al

        ; восстановление idtr
		lidt    fword ptr ipdescr16
        mov ax, 3FFh ; граница таблицы векторов (1кб)
        mov word ptr ipdescr, ax
        mov eax, 0 ; лин базовый адрес
        mov dword ptr ipdescr+2, eax 
        lidt ipdescr

        ; закрытие линии A20
        mov al, 0D1h 
        out 64h, al
        mov al, 0DDh
        out 60h, al
		
		;Разрешение прерываний
		sti
		xor al, al
		out 70h, al

        ; очистить экран
        mov ax, 3
        int 10h

        mov ah, 09h
        lea dx, pm_msg_out
        int 21h
        xor dx, dx
        mov ah, 2
        mov dl, 13
        int 21h
        mov dl, 10
        int 21h

        mov ax, 4C00h
        int 21h
;code_size = $-start  
code ends

end start

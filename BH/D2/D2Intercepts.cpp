#include "D2Handlers.h"
#include "D2Ptrs.h"

VOID __declspec(naked) GameDraw_Interception()
{
	__asm
	{
		call GameDraw;

		//POP ESI
		POP EBX
		POP ECX
		RETN 4
	}
}

void __declspec(naked) GameAutomapDraw_Interception()
{
   __asm
   {
      push eax;
      call GameAutomapDraw;
      pop eax;
      pop edi;
      pop esi;
      ret;
   }
} 

VOID __declspec(naked) OOGDraw_Interception()
{
	__asm
	{
		PUSHAD
		CALL OOGDraw
		POPAD
		MOV ESI,DWORD PTR DS:[ESI+0x3C]
		TEST ESI, ESI
		RETN
	}
}

void __declspec(naked) GameLoop_Interception()
{
	__asm
	{
		pushad;
		call GameLoop;
		popad;
 
		pop eax;
		sub esp, 0x20;
		mov [esp + 0xC], ecx;
		push eax;
		ret;
	}
}

VOID __declspec(naked) GamePacketRecv_Interception() {
	__asm
	{
		pop ebp;
		pushad;

		call GamePacketRecv;
		test eax, eax;

		popad;
		jnz OldCode;

		mov edx, 0;

OldCode:
		call D2NET_ReceivePacket_I;

		push ebp;
		ret;
	}
}

VOID __declspec(naked) ChatPacketRecv_Interception()
{
   __asm
   {
      pushad;
      call ChatPacketRecv;
      TEST EAX,EAX;

      popad;
      jnz oldCall;

        MOV EAX,0;
      MOV DWORD PTR DS:[EBX+0x6FF3F100],1
      ret;
oldCall:
        CALL eax;
        MOV DWORD PTR DS:[EBX+0x6FF3F100],1
        ret;

   }
}

VOID __declspec(naked) RealmPacketRecv_Interception()
{
	__asm
	{
		LEA ECX,DWORD PTR SS:[ESP+4]
		PUSHAD
		CALL RealmPacketRecv
		CMP EAX, 0
		POPAD
		JE Block
		CALL EAX
Block:
		RETN
	}
}

void __declspec(naked) GameInput_Interception(void)
{
	__asm {
		pushad
		mov ecx, ebx
		call GameInput
		cmp eax, -1
		popad
		je BlockIt
		call D2CLIENT_InputCall_I
		ret

BlockIt:
		xor eax,eax
		ret
	}
}

void __declspec(naked) ChannelInput_Interception(void)
{
	__asm
	{
		push ecx
		mov ecx, esi

		call ChannelInput

		test eax, eax
		pop ecx

		jz SkipInput
		call D2MULTI_ChannelInput_I

SkipInput:
		ret
	}
}

void __declspec(naked) ChannelWhisper_Interception(void)
{
	__asm
	{
		push ecx
		push edx
		mov ecx, edi
		mov edx, ebx

		call ChatHandler

		test eax, eax
		pop edx
		pop ecx

		jnz SkipWhisper
		jmp D2MULTI_ChannelWhisper_I

SkipWhisper:
		ret 4
	}
}

void __declspec(naked) ChannelChat_Interception(void)
{
	__asm
	{
		push ecx
		push edx
		mov ecx, dword ptr ss:[esp+0xC]
		mov edx, dword ptr ss:[esp+0x10]

		call ChatHandler

		test eax, eax
		pop edx
		pop ecx

		jnz SkipChat
		sub esp, 0x308
		jmp D2MULTI_ChannelChat_I

SkipChat:
		ret 8
	}
}

void __declspec(naked) ChannelEmote_Interception(void)
{
	__asm
	{
		push ecx
		push edx
		mov ecx, dword ptr ss:[esp+0xC]
		mov edx, dword ptr ss:[esp+0x10]

		call ChatHandler

		test eax, eax
		pop edx
		pop ecx

		jnz SkipChat
		sub esp, 0x4F8
		jmp D2MULTI_ChannelEmote_I

SkipChat:
		ret 8
	}
}

void __declspec(naked) PositionMenuTab_Interception(void) {
	__asm
	{
		; Check that the mode is set to HD, otherwise run 640 mode code
		cmp eax, 03

		pushad
		mov eax, p_D2CLIENT_TabXOffset
		jne PositionMenuTab_Mode640

		; HD positions
		; Position X offset
		mov dword ptr ds:[eax], 352
		; Position Y offset
		mov dword ptr ds:[eax+4], -110

		jmp PositionMenuTab_RepositionEnd

PositionMenuTab_Mode640:
		mov dword ptr ds:[eax], 0
		mov dword ptr ds:[eax+4], 0

PositionMenuTab_RepositionEnd:
		popad
		ret
	}
}
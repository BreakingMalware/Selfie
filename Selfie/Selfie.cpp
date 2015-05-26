#include "stdafx.h"

DWORD  orig_import_table_size = 0;
app_pc main_mdl_start_addr = 0;
app_pc main_mdl_end_addr = 0;
app_pc main_mdl_entry_point = 0;

DWORD GetImportAddressTableSize(void);
dr_emit_flags_t on_event_basic_block(void *drcontext, void *tag, instrlist_t *bb, bool for_trace, bool translating);
void on_call_event(app_pc caller, app_pc callee);


DR_EXPORT void
dr_init(client_id_t id)
{
	//Looks up module data for the main executable.
	module_data_t *mod = dr_get_main_module();

	//main executable start address
	main_mdl_start_addr = mod->start;

	//main executable end address
	main_mdl_end_addr = mod->end;

	//main executable entry point
	main_mdl_entry_point = mod->entry_point;

	//main executable IAT orig size
	orig_import_table_size = GetImportAddressTableSize();

	//registers a callback function for the basic block event.
	dr_register_bb_event(on_event_basic_block);

	//cleanup
	dr_free_module_data(mod);
}



dr_emit_flags_t on_event_basic_block(void *drcontext, void *tag, instrlist_t *bb,
	bool for_trace, bool translating)
{
	instr_t *instr, *next_instr;

	for (instr = instrlist_first(bb); instr != NULL; instr = next_instr) {
		next_instr = instr_get_next(instr);
		if (!instr_opcode_valid(instr))
			continue;

		if (instr_is_call_direct(instr)) {
			dr_insert_call_instrumentation(drcontext, bb, instr, (app_pc)on_call_event);
		}
		else if (instr_is_call_indirect(instr)) {
			dr_insert_mbr_instrumentation(drcontext, bb, instr, (app_pc)on_call_event,
				SPILL_SLOT_1);
		}
	}

	return DR_EMIT_DEFAULT;

}

void 
on_call_event(
	app_pc caller,
	app_pc callee)
{
	if (main_mdl_start_addr <= callee && callee <= main_mdl_end_addr)
	{
		DWORD current_iat_size = GetImportAddressTableSize();

		if (current_iat_size != orig_import_table_size)
		{
			dr_messagebox(". .. ... Selfie ... .. .\n\nTarget Application: %s\nTarget PID: %d\n\nOrig Application Entry Point: 0x%.4x\nOrig Application IAT Size: 0x%.4x (%d)\n\nUnpacked IAT Size: 0x%.4x (%d)\nSuspected OEP: 0x%.4x\n\n\t Now dump and enjoy reversing.....:)", dr_get_application_name(), dr_get_process_id(), main_mdl_entry_point, orig_import_table_size, orig_import_table_size, current_iat_size, current_iat_size, callee);
			dr_exit_process(ERROR_SUCCESS);
		}
	}
}


DWORD
GetImportAddressTableSize(
	void)
{
	HMODULE hModule = GetModuleHandle(NULL);

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;

	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)pDosHeader + pDosHeader->e_lfanew);

	return pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
}
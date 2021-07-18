////////////////////////////////
// NOTE(allen): Temp Wave

function String8
temp_wave_file_name(M_Arena *arena){
    M_Scratch scratch(arena);
    String8 temp_path = os_file_path(scratch, OS_SystemPath_TempData);
    String8 result = str8_pushf(arena, "%.*s\\hqps.wav", str8_expand(temp_path));
    return(result);
}

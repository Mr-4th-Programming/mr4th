////////////////////////////////
// NOTE(allen): Temp Wave

function String8
temp_wave_file_name(M_Arena *arena){
    M_ArenaTemp scratch = m_get_scratch(&arena, 1);
    String8 temp_path = os_file_path(scratch.arena, OS_SystemPath_TempData);
    String8 result = str8_pushf(arena, "%.*s\\hqps.wav", str8_expand(temp_path));
    m_release_scratch(scratch);
    return(result);
}

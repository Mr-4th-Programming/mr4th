////////////////////////////////
// NOTE(allen): Files

function B32
os_file_write(String8 file_name, String8 data){
    String8Node node = {};
    String8List list = {};
    str8_list_push_explicit(&list, data, &node);
    B32 result = os_file_write(file_name, list);
    return(result);
}


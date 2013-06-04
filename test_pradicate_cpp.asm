			if(test_predicate(array_ptr + i, range_filters)) 

// !range_filters->use_filter[age_e] ||  
// Optimized and reduced: !range_filters->use_filter[age_e] ||  
// 
// (row->age >= range_filters->begin.age && row->age <= range_filters->end.age)
000000013FE03520  mov         edx,dword ptr [r14+r10*8]  
000000013FE03524  mov         r9d,dword ptr [rdi]  
000000013FE03527  mov         r8d,edx  // save: edx
000000013FE0352A  mov         eax,r9d  // save: r9d
000000013FE0352D  shr         r8d,15h  
000000013FE03531  shr         eax,15h  
000000013FE03534  and         r8d,7Fh  
000000013FE03538  and         eax,7Fh  
000000013FE0353B  cmp         r8d,eax  
000000013FE0353E  jb          T_custom_filter<12>::search+88h (013FE03578h)  
000000013FE03540  mov         ecx,dword ptr [rdi+8]  
000000013FE03543  mov         eax,ecx  
000000013FE03545  shr         eax,15h  
000000013FE03548  and         eax,7Fh  
000000013FE0354B  cmp         r8d,eax  
000000013FE0354E  ja          T_custom_filter<12>::search+88h (013FE03578h)  
// Optimized and reduced: !range_filters->use_filter[code_e] ||  
// 
// No need to load: edx, r9d (C-edition: ecx, eax), because they were been saved
//
// (row->code >= range_filters->begin.code && row->code <= range_filters->end.code)
000000013FE03550  and         edx,0FFFFFh  
000000013FE03556  and         r9d,0FFFFFh  
000000013FE0355D  cmp         edx,r9d  
000000013FE03560  jb          T_custom_filter<12>::search+88h (013FE03578h)  
// 
000000013FE03562  and         ecx,0FFFFFh  
000000013FE03568  cmp         edx,ecx  
000000013FE0356A  ja          T_custom_filter<12>::search+88h (013FE03578h)  
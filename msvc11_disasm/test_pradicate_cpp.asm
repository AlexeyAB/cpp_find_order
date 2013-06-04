		for(i = 0; i < c_array_size; ++i) {
000000013F533508  mov         r10d,r11d  
000000013F53350B  test        r8,r8  
000000013F53350E  je          T_custom_filter<12>::search+95h (013F533585h)  
000000013F533510  mov         qword ptr [this],rdi  
000000013F533515  mov         rdi,qword ptr [range_filters]  
000000013F53351A  nop         word ptr [rax+rax]  
			if(test_predicate(array_ptr + i, range_filters)) 
000000013F533520  mov         edx,dword ptr [r14+r10*8]  
000000013F533524  mov         r9d,dword ptr [rdi]  
000000013F533527  mov         r8d,edx  
000000013F53352A  mov         eax,r9d  
000000013F53352D  shr         r8d,15h  
000000013F533531  shr         eax,15h  
000000013F533534  and         r8d,7Fh  
000000013F533538  and         eax,7Fh  
			if(test_predicate(array_ptr + i, range_filters)) 
000000013F53353B  cmp         r8d,eax  
000000013F53353E  jb          T_custom_filter<12>::search+88h (013F533578h)  
000000013F533540  mov         ecx,dword ptr [rdi+8]  
000000013F533543  mov         eax,ecx  
000000013F533545  shr         eax,15h  
000000013F533548  and         eax,7Fh  
000000013F53354B  cmp         r8d,eax  
000000013F53354E  ja          T_custom_filter<12>::search+88h (013F533578h)  

000000013F533550  and         edx,0FFFFFh  
000000013F533556  and         r9d,0FFFFFh  
000000013F53355D  cmp         edx,r9d  
000000013F533560  jb          T_custom_filter<12>::search+88h (013F533578h)  
000000013F533562  and         ecx,0FFFFFh  
000000013F533568  cmp         edx,ecx  
000000013F53356A  ja          T_custom_filter<12>::search+88h (013F533578h)  
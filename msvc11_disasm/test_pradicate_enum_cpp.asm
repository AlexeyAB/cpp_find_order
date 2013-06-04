		T_unroll_functor<T_row::last_e, T_test_pred> test_predicate;
		for(i = 0; i < c_array_size; ++i) {
000000013F3E3508  mov         r10d,r11d  
000000013F3E350B  test        r8,r8  
000000013F3E350E  je          T_custom_filter<T_cash_account_row,12>::search+95h (013F3E3585h)  
000000013F3E3510  mov         qword ptr [this],rdi  
000000013F3E3515  mov         rdi,qword ptr [range_filters]  
000000013F3E351A  nop         word ptr [rax+rax]  
			if(test_predicate(array_ptr + i, range_filters)) 
000000013F3E3520  mov         r9d,dword ptr [rdi]  
000000013F3E3523  mov         edx,dword ptr [r14+r10*8]  
000000013F3E3527  mov         r8d,edx  
000000013F3E352A  mov         eax,r9d  
000000013F3E352D  shr         r8d,15h  
000000013F3E3531  shr         eax,15h  
000000013F3E3534  and         r8d,7Fh  
000000013F3E3538  and         eax,7Fh
			if(test_predicate(array_ptr + i, range_filters)) 
000000013F3E353B  cmp         r8d,eax  
000000013F3E353E  jb          T_custom_filter<T_cash_account_row,12>::search+88h (013F3E3578h)  
000000013F3E3540  mov         ecx,dword ptr [rdi+8]  
000000013F3E3543  mov         eax,ecx  
000000013F3E3545  shr         eax,15h  
000000013F3E3548  and         eax,7Fh  
000000013F3E354B  cmp         r8d,eax  
000000013F3E354E  ja          T_custom_filter<T_cash_account_row,12>::search+88h (013F3E3578h)  

000000013F3E3550  and         edx,0FFFFFh  
000000013F3E3556  and         r9d,0FFFFFh  
000000013F3E355D  cmp         edx,r9d  
000000013F3E3560  jb          T_custom_filter<T_cash_account_row,12>::search+88h (013F3E3578h)  
000000013F3E3562  and         ecx,0FFFFFh  
000000013F3E3568  cmp         edx,ecx  
000000013F3E356A  ja          T_custom_filter<T_cash_account_row,12>::search+88h (013F3E3578h) 
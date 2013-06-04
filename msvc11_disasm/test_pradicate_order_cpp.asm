		T_unroll_functor<T_row::last_e, T_test_pred> test_predicate;
		for(i = 0; i < c_array_size; ++i) {
000000013FC9E608  mov         r10d,r11d  
000000013FC9E60B  test        r8,r8  
000000013FC9E60E  je          T_custom_filter<T_cash_account_row,12,58>::search+93h (013FC9E683h)  
000000013FC9E610  mov         qword ptr [this],rbx  
000000013FC9E615  mov         rbx,qword ptr [range_filters]  
000000013FC9E61A  nop         word ptr [rax+rax]
			if(test_predicate(array_ptr + i, range_filters)) 
000000013FC9E620  mov         r9d,dword ptr [rbx]  
000000013FC9E623  mov         edx,dword ptr [r14+r10*8]  
000000013FC9E627  mov         r8d,edx  
000000013FC9E62A  mov         eax,r9d  

000000013FC9E62D  and         r8d,0FFFFFh  
000000013FC9E634  and         eax,0FFFFFh  
000000013FC9E639  cmp         r8d,eax  
000000013FC9E63C  jb          T_custom_filter<T_cash_account_row,12,58>::search+86h (013FC9E676h)  
000000013FC9E63E  mov         ecx,dword ptr [rbx+8]  
000000013FC9E641  mov         eax,ecx  
000000013FC9E643  and         eax,0FFFFFh  
000000013FC9E648  cmp         r8d,eax  
000000013FC9E64B  ja          T_custom_filter<T_cash_account_row,12,58>::search+86h (013FC9E676h)  
000000013FC9E64D  shr         edx,15h  
000000013FC9E650  shr         r9d,15h  
000000013FC9E654  and         edx,7Fh  
000000013FC9E657  and         r9d,7Fh  
000000013FC9E65B  cmp         edx,r9d  
000000013FC9E65E  jb          T_custom_filter<T_cash_account_row,12,58>::search+86h (013FC9E676h)  
000000013FC9E660  shr         ecx,15h  
000000013FC9E663  and         ecx,7Fh  
000000013FC9E666  cmp         edx,ecx  
000000013FC9E668  ja          T_custom_filter<T_cash_account_row,12,58>::search+86h (013FC9E676h)  
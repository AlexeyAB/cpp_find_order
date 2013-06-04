		if(test_predicate(array_ptr + i, range_filters)) 
000000013F6211A3  mov         eax,dword ptr [r9+0Ch]  
000000013F6211A7  and         eax,0FFFFFh  
000000013F6211AC  cmp         ecx,eax  
000000013F6211AE  ja          search+10Fh (013F62127Fh)  
000000013F6211B4  cmp         byte ptr [r9+11h],0  
000000013F6211B9  je          search+78h (013F6211E8h)  
000000013F6211BB  mov         ecx,dword ptr [r10+rdx*8]  
000000013F6211BF  mov         eax,dword ptr [r9]  
000000013F6211C2  shr         ecx,14h  
000000013F6211C5  shr         eax,14h  
000000013F6211C8  and         ecx,1  
000000013F6211CB  and         eax,1  
000000013F6211CE  cmp         ecx,eax  
000000013F6211D0  jb          search+10Fh (013F62127Fh)  
000000013F6211D6  mov         eax,dword ptr [r9+8]  
000000013F6211DA  shr         eax,14h  
000000013F6211DD  and         eax,1  
000000013F6211E0  cmp         ecx,eax  
000000013F6211E2  ja          search+10Fh (013F62127Fh)  
// !range_filters->use_filter[age_e] ||  
000000013F6211E8  cmp         byte ptr [r9+12h],0  
000000013F6211ED  je          search+0A4h (013F621214h)  
// (row->age >= range_filters->begin.age && row->age <= range_filters->end.age)
000000013F6211EF  mov         ecx,dword ptr [r10+rdx*8]  
000000013F6211F3  mov         eax,dword ptr [r9] 

 
000000013F6211F6  shr         ecx,15h  
000000013F6211F9  shr         eax,15h  
000000013F6211FC  and         ecx,7Fh  
000000013F6211FF  and         eax,7Fh  
000000013F621202  cmp         ecx,eax  
000000013F621204  jb          search+10Fh (013F62127Fh)  
000000013F621206  mov         eax,dword ptr [r9+8]  

000000013F62120A  shr         eax,15h  
000000013F62120D  and         eax,7Fh  
000000013F621210  cmp         ecx,eax  
000000013F621212  ja          search+10Fh (013F62127Fh)  
000000013F621214  cmp         byte ptr [r9+13h],0  
000000013F621219  je          search+0CEh (013F62123Eh)  
000000013F62121B  mov         ecx,dword ptr [r10+rdx*8]  
000000013F62121F  mov         eax,dword ptr [r9]  
// (row->code >= range_filters->begin.code && row->code <= range_filters->end.code)
000000013F621222  and         ecx,0FFFFFh  
000000013F621228  and         eax,0FFFFFh  
000000013F62122D  cmp         ecx,eax  
000000013F62122F  jb          search+10Fh (013F62127Fh)  
000000013F621231  mov         eax,dword ptr [r9+8]  
000000013F621235  and         eax,0FFFFFh  
000000013F62123A  cmp         ecx,eax  
000000013F62123C  ja          search+10Fh (013F62127Fh)  
000000013F62123E  cmp         byte ptr [r9+14h],0  
000000013F621243  je          search+103h (013F621273h)  
000000013F621245  mov         ecx,dword ptr [r10+rdx*8+4]  
000000013F62124A  mov         eax,dword ptr [r9+4]  
000000013F62124E  shr         ecx,14h  
000000013F621251  shr         eax,14h  
000000013F621254  and         ecx,1FFh  
000000013F62125A  and         eax,1FFh  
000000013F62125F  cmp         ecx,eax  
000000013F621261  jb          search+10Fh (013F62127Fh)  
000000013F621263  mov         eax,dword ptr [r9+0Ch]  
000000013F621267  shr         eax,14h  
000000013F62126A  and         eax,1FFh  
000000013F62126F  cmp         ecx,eax  
000000013F621271  ja          search+10Fh (013F62127Fh)  

import sys, os, os.path, time

CL_OPTS   = (['-nologo','-O2'],['-nologo','-O2','-D_FORCEINLINE'])
GCC_OPTS  = (['-O3'],['-O3','-D_FORCEINLINE'])
GCC3_OPTS = (['-O3'],)
COMPILER_HOME = 'c:/OPt/C++/'
COMPILERS = [ 
            ( 'MSVC7.1', 'cl.exe', CL_OPTS, '-Fe',
                ( [COMPILER_HOME+r'7.1/bin'],
                  [COMPILER_HOME+r'7.1/include',COMPILER_HOME+r'PSDK71/include'],
                  [COMPILER_HOME+r'7.1/lib',COMPILER_HOME+r'PSDK71/lib'] ) ),
            ( 'MSVC10.1', 'cl.exe', CL_OPTS, '-Fe',
                ( [COMPILER_HOME+r'10.1/bin'],
                  [COMPILER_HOME+r'10.1/include',COMPILER_HOME+r'PSDK71/include'],
                  [COMPILER_HOME+r'10.1/lib',COMPILER_HOME+r'PSDK71/lib'] ) ),
            ( 'MSVC11.2', 'cl.exe', CL_OPTS, '-Fe',
                ( [COMPILER_HOME+r'11.2/bin'],
                  [COMPILER_HOME+r'11.2/include',COMPILER_HOME+r'PSDK71/include'],
                  [COMPILER_HOME+r'11.2/lib',COMPILER_HOME+r'PSDK71/lib'] ) ),
            ( 'GCC3', 'gcc.exe', GCC3_OPTS, '-o',
                ( [COMPILER_HOME+r'gcc-3/bin'],
                  [],
                  [] ) ),
            ( 'GCC4', 'gcc.exe', GCC_OPTS, '-o',
                ( [COMPILER_HOME+r'gcc-4/bin'],
                  [],
                  [] ) ),
            ( 'CLANG3', 'clang.exe', GCC_OPTS, '-o',
                ( [COMPILER_HOME+r'gcc-4/bin'],
                  [],
                  [] ) ),
            ( 'ICL12.1', 'icl.exe', CL_OPTS, '-Fe',
                ( [COMPILER_HOME+r'icl-12.1/bin',COMPILER_HOME+r'10.1/bin'],
                  [COMPILER_HOME+r'icl-12.1/include',COMPILER_HOME+r'10.1/include',COMPILER_HOME+r'PSDK71/include'],
                  [COMPILER_HOME+r'icl-12.1/lib',COMPILER_HOME+r'10.1/lib',COMPILER_HOME+r'PSDK71/lib'] ) ),                
           ]

artifacts = []

def compile():
    
    env = [ 
        ('PATH',   os.getenv('PATH','')),
        ('INCLUDE',os.getenv('INCLUDE','')),
        ('LIB',    os.getenv('LIB','')) 
    ]
    
    for cc in COMPILERS:
        cmtm = os.path.getmtime('main.c')
        opts = cc[2]
        for i in range(len(opts)):
            tm = time.time()
            target = cc[0]+'-'+str(i)+'.exe'
            CC = cc[1]+' main.c '+' '.join(opts[i])+' '+cc[3]+target
            if not os.path.exists(target) or os.path.getmtime(target) <= cmtm:
                for e in range(len(env)):
                    l = ';'.join(map(os.path.normpath,cc[4][e])+[env[e][1]])
                    os.putenv(env[e][0],l)
                os.popen(CC).readline()
            buildtime = time.time()-tm
            artifacts.append(target)
            print '%-8.3f'%buildtime, ':', CC     

def perform(n):
    L = dict( map(lambda x: (x,[]), artifacts) )
    for target in artifacts:
        for i in range(n):
            S = os.popen(target).readline()
            count, found, fast, slow, dif = map(float,S.split())
            L[target].append((fast,slow))
            sys.stderr.write('.')
    sys.stderr.write('\n')
    for target in artifacts:
        x,y = reduce(lambda x,y: (min(x[0],y[0]),min(x[1],y[1])), L[target])
        print '%-20s'%target, '%8.3f'%x, '%8.3f'%y, '%8.3f'%(y/x)   

compile()
perform(int(sys.argv[1]) if len(sys.argv)>1 else 3)



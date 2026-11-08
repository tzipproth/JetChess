import subprocess,re,json,time,argparse
from pathlib import Path
root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser()
parser.add_argument('--configuration',choices=['Release','Debug'],default='Release')
config=parser.parse_args().configuration
probe=root/'build/tests'/config/'engine_probe.exe'
baseline=json.loads((root/'tests/expected-results.json').read_text(encoding='utf-8'))
def case_key(row):
    return (row['fen'],row['mode'],row['depth'],row['hash_mb'],row['steps'])
references={case_key(row):row['result'] for row in baseline}
report=[]
def run(exe,args):
    p=subprocess.run([str(exe),*map(str,args)],cwd=root,capture_output=True,text=True,errors='replace',timeout=180)
    if p.returncode: raise RuntimeError((args,p.returncode,p.stderr))
    return p.stdout

def signature(output,mode):
    if mode in 'MP':
        m=re.search(r'(\d[\d.]*)\s+\((?:Move pathes|Positions) after',output)
        assert m,output
        return m[1].replace('.','')
    if mode=='B':
        assert 'Benchmark Time:' in output,output
        assert len(re.findall(r'Time [1-5]:',output))==5,output
        return 'five benchmark positions completed'
    matches=re.findall(r'^(?:Solution:.*|Multiple Solutions|No mate in.*)$',output,re.M)
    assert matches,output
    return matches[-1].strip()

cases=[]
for depth,value in enumerate(([20,400,8902,197281,4865609] if config=='Debug' else [20,400,8902,197281,4865609,119060324,3195901860,84998978956]),1):
    cases.append(('a.fen','M',depth,1024,1,str(value)))
for depth,value in enumerate([48,2039,97862,4085603,193690690],1):
    cases.append(('p.fen','M',depth,1024,1,str(value)))
for f in ['test1.fen','ep1.fen','pf01.fen','n.fen']:
    cases.append((f,'M',4,1024,1,None))
for depth in [1,2,3,4]:
    cases.append(('a.fen','P',depth,1024,1,None))
for steps in [2,4,8,16,32]:
    cases.append(('a.fen','P',3,1024,steps,None))
cases.append(('ep1.fen','P',3,1024,1,None))
for f,depth in [('mat101.fen',1),('mat201.fen',2),('mat301.fen',3),('mat505.fen',5),('a.fen',1)]:
    cases.append((f,'S',depth,1024,1,None))
if config=='Release':
    cases.append(('a.fen','P',6,1024,1,'9417681'))
    cases.append(('a.fen','B',1,1024,1,None))
for f,mode,depth,size,steps,expected in cases:
    args=[root/'tests/fixtures'/f,mode,depth,size,steps]
    start=time.monotonic()
    actual=signature(run(probe,args),mode)
    reference=references[(f,mode,depth,size,steps)]
    assert actual==reference,(args,actual,reference)
    if expected: assert actual==expected,(args,actual,expected)
    item=dict(fen=f,mode=mode,depth=depth,hash_mb=size,steps=steps,result=actual,seconds=round(time.monotonic()-start,3))
    report.append(item)
    print(f'PASS {mode} {f} depth={depth} hash={size} steps={steps}: {actual}',flush=True)
(root/f'build/regression-results-{config}.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(f'{len(report)} cases passed against recorded reference results.')

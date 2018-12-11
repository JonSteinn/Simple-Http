#include "simple_http/simple_http_app.h"

#define RouteHandler(name, body) void name(__attribute__((unused)) Request* req, __attribute__((unused)) Response* res, __attribute__((unused)) int32_t argc, __attribute__((unused)) char** argv) body
RouteHandler(t1, {    
    res->status_code = status_code.OK;
    g_hash_table_insert(res->headers, g_strdup("Content-Type"), g_strdup("text/html; charset=utf-8"));
    g_string_append(res->body, "<!DOCTYPE html><html><head><title>Test-Title</title></head><body>Testing...1<br><a href=\"/index.html\">click me</a></body></html>");    
    GString* str = g_string_new(NULL);
    g_string_append_printf(str, "%zu", res->body->len);
    g_hash_table_insert(res->headers, g_strdup("Content-Length"), g_strdup(str->str));
    g_string_free(str, true);
})
RouteHandler(t2, {    
    res->status_code = status_code.OK;
    g_hash_table_insert(res->headers, g_strdup("Content-Type"), g_strdup("text/html; charset=utf-8"));
    g_string_append(res->body, "<!DOCTYPE html><html><head><title>Test-Title</title></head><body>Testing...2<br><a href=\"/a/b/jon/c/steinn\">click me</a></body></html>");    
    GString* str = g_string_new(NULL);
    g_string_append_printf(str, "%zu", res->body->len);
    g_hash_table_insert(res->headers, g_strdup("Content-Length"), g_strdup(str->str));
    g_string_free(str, true);
})
RouteHandler(t3, {    
    res->status_code = status_code.OK;
    g_hash_table_insert(res->headers, g_strdup("Content-Type"), g_strdup("text/html; charset=utf-8"));
    g_string_append_printf(res->body, "<!DOCTYPE html><html><head><title>Test-Title</title></head><body>%s and %s<br><a href=\"/something\">click me</a></body></html>", argv[0], argv[1]);
    GString* str = g_string_new(NULL);
    g_string_append_printf(str, "%zu", res->body->len);
    g_hash_table_insert(res->headers, g_strdup("Content-Length"), g_strdup(str->str));
    g_string_free(str, true);
})
RouteHandler(t4, {    
    res->status_code = status_code.OK;
    g_hash_table_insert(res->headers, g_strdup("Content-Type"), g_strdup("text/html; charset=utf-8"));
    g_string_append_printf(res->body, "<!DOCTYPE html><html><head><title>Test-Title</title></head><body>%s</body></html>", argv[0]);
    GString* str = g_string_new(NULL);
    g_string_append_printf(str, "%zu", res->body->len);
    g_hash_table_insert(res->headers, g_strdup("Content-Length"), g_strdup(str->str));
    g_string_free(str, true);
})

int32_t main(int32_t argc, char** argv) {
    Server server;
    config_server(&server, "./src/config.txt", argc, argv);
    
    // Add routes here
    add_route(&server, METHOD_GET, "", &t1);
    
    Response* r = create_empty_response();
    g_hash_table_insert(r->headers, g_strdup("Content-Type"), g_strdup("text/html"));
    g_string_append(r->body, "<!DOCTYPE html><html><head><title>OVERRIDE</title></head><body>X</body></html>");
    override_default_response(&server, r, status_code.NOT_FOUND, true);
    //add_route(&server, METHOD_GET, "a/b", &t2);
    //add_route(&server, METHOD_GET, "a/b/{arg}/c/{arg}", &t3);
    //add_route(&server, METHOD_GET, "{arg}", &t4);

    start_server(&server);
    destroy_server(&server);
    return 0;
}

/*
prefix-hash-for-def

tpOB36F1uj
tZWPInUiHh
Up6MspZrDp
wgtEL3FD4V
ZIlhpZFzQD
HfwT8zFjfu
E9sYe5WFvi
TJyiLq9nRs
diYyFKtLgV
xewauJG3uG
TJ8l9OzHaM
MVFyTdJ3Ds
cp4Anj93fD
CxrbrqhrvX
QhhOPV3BTN
dP1Z8UQ24P
TS91kPProT
yoZBa8N6Kv
xk1yPRTTFr
QsRug532UX
FxJYmvEJLC
hcw6LInGCE
DC00LrdRuE
KmhfP6aZIU
sAjU5bS2Q2
aF04NGFpoT
cij6XhnfUW
jTLijBUVE9
JquIP4H9DD
bCa8IJIynB
SGnk2MA356
lcy2qhfHoT
Dthue8uFGc
MJnLHlbuh4
t6fgaRJclD
V6ousZL6Pt
nUsjDPt2Jd
OQKyd6stoj
H8naW98OHO
oN9vz94MGI
dtOJP71yAE
Wn1BDuc5nn
hA7hSVPREh
WQGQbBpdmG
DXW2V6dv89
tTbbjnZ5oN
sC8s7fTz6R
uj7mL5sRE0
vsbZ4kYVw7
ILc124aLmU
l9o3bz8Anh
ORkBT5jLZb
cBIh1glOKZ
lVfGudT63O
umnGsPYlrB
SdaNAyYRgz
O6MHt9xjV2
bLk7f1GY1V
O5tSYh3jvl
uUv0mMp5wJ
i51qYNJIze
AbpZDfKeiK
t0Ep2nqo3w
ca9kKjh4iF
HGPhYOgT6u
yvykhDnp5W
dc9MhtMBLX
Bo1N0EorMH
BnuXGEnNfW
GUWJlEfP3s
NudbqoSd9G
Vsc3gbT74F
h1bsxatMPA
qCAMjRUhQb
amDFXBoVZD
G85vQNM6k0
oWIxav3XL1
FaQfqV79SL
jBthGeKCll
kKkT0OZ7kd
YKt5xyilOJ
rL7cBsuS17
NXsyziSsoa
Zb7veMjEq6
S54l2K9wPL
jGrpAubZqa
xK5ozrgKMD
Vz3fnmc0tT
YWan9fFO5S
kZOpquoQZN
sTDJ5c2OkU
HsJF0TcZLN
y4QQctC0Gc
q31qliUVeI
K7ToC3QfCF
sRxpwwoCS6
gBaPxr4X1R
H6bkmI5h0M
vIukHwb8hT
bhiuWJWC6r
hi2yLtPi08
MbfEjFcdah
XXb9Xv5dBg
g2j6MoLwXL
kWFzb6KU1V
PJ97yWN8N5
BMRYprdiWo
hpyncc2buP
zP0EzEZvWa
XPRiK42amI
jSNH2Osqce
B2F9tjG6wb
cxpLjqwV4r
bEBa05CFKG
aYvrTvN31G
flQHc6tEBE
JoCgE5tdod
oB9mzc6O9x
CpklQdsDRG
xNLPVUK1Kx
RLgNC5ucDI
Z426To3Iyu
ppaYppKYVf
B8vtfNfdNe
trgGZyT1dD
Qf6uRBHgPD
t2NHjeqnIv
rFBdhRkuZG
FBsf4F8fdg
GkUMNBD5sp
dws0tSKLHs
I3ymUuW5AJ
se347y36Br
t17UQWIVtc
CfnExXZdyo
P8u6d5X72M
KYMxBYQ2E1
WPEnE2YFyA
pFQlxYUXhj
YhLqNphIrZ
RDgIcL00Zb
CNzlQQufPy
PzY4rvDG50
wxTqVnxzi2
uPPsDoo2VH
qWXrEaZBit
X0wsRTpKFI
Uyu7CZbSme
xV1wtrJ9es
hkNe9MzwdA
ncwe4wqj6W
SFKVm311Du
udicpJ6OlX
dPE62Dx7GP
y2CkMgSqDg
dJ4PuyVCpQ
PWHbsa0CEB
baFCCzbXzA
cL6rhHeR1Q
RUiwby9Hvx
B6QIFNqbEk
KC1gQhWkTA
ntZKvS1uC2
TK4cN4k2iD
qJYJ22uYns
wtCa0hp6n1
acanbUNMjz
dr0oxQ4Ji2
wV6anXCrgC
owJSni6FNG
TfjNRqksV4
Ba1wcgPfJM
mLrxgcJd31
guVyH8O1jR
ue0JCeM2IW
qZtPpTYB1A
ifwUOma0jJ
zNEaIWypcz
oLzl3Dqha3
GKbbDTCDLW
fBDSgqgPHw
VAbrJQeYva
HOvYrhI3EE
fexJCo94iu
EAqJahAKxU
hrZtdNaCwY
aradoEl0yG
MoZ7IEQn8n
OinSf7VrBV
FKU8A6m1Ho
QgU2a4gsJ9
EM9wbQB0iF
YlC4oFeddR
h1aJEQIFCV
Hc8SkUVXDl
EQvESmigI8
aJM67Tbvmk
mgsI4TugX9
OCpiXKd9sF
KGBGBDdtIW
Bbbx9w63Pq
YQR5VXZigT
vmWBJMeCp4
WvzMrn2D7y
T2VieGoxSq
XK30lw17k7
EeEbRXdsGo
NRzw0aIwHx
RgEo0VrK2t
MF6YsxDwfP
fZR9x4LS4a
MWArHDMofF
aQZDSAnNV8
BEfOsoyyJ8
v7adHnQOHP
l6Bgjdzpqm
XpZiZ147zp
uJCG9hYo3l
ZZY1GX2Mtf
FF5tFktDRF
IDhNPoNAX9
g89N5oDRsy
a2xqs3UxZn
GyW8nxyMwU
Ll3sV8fdnL
cuwCIj039n
pIsx4aXjVE
mk6MHwKSpt
ohR9qnIexJ
n0ywbU9Gkg
NVp9WQswJM
pwLqHs1XT2
L3Te2OdYEc
DLl400hkKh
ZNvwXhL2hh
GzBMrZf1SR
ex9De4PhiW
GYH6lJADGY
FL2rb9a7cU
EJE0FN8t7g
QX1ooBxhVA
quE43sSuEx
CDILiI8W9w
U6EOg2DFag
jhcnEDAS3v
bT7TxYGGsR
g0Tn0IjwlU
febtrXpNRE
qPjeqhFHvJ
mEy0KOFPjU
C4nlemO1xl
oqkNuE95By
hMngTuTTn2
AQru8hLTre
WeICiRd60j
Wfzt4nmMLm
jH9K9wPCU9
BPSbhB1x6w
bHXclJKzmI
dZF67i899o
PMtJO3th6P
xDkFUBcnbY
GgG4jCDNYb
XsLKzC1hIs
guhMr9mloj
ECu6FifxOp
cbCdF04en5
HiKUA5vdhf
b22umyrIQP
WDS4F8m5Vl
A29Fe4mwHu
oFiNuCkMFC
BrybZaEexc
VKx9liSTIx
CcB9qCf6pZ
IpstkTv4M8
WMPrZdArAI
ymRwkTZipP
MXwdSnDv17
P5obgbWmCm
CZxjVWbwkc
MGo0kcfLhK
zOli4w0cwP
vGbx1LMliE
LK5A4sfjVa
AuVxlMOYZ9
DSpU8LYDbq
x7hC186S12
g3yYZkvXSd
emIsbeTOUH
DEZpApI325
QhuI34zY26
Kr56HzZESx
lsjTh6dWG2
wwxZTLaxU1
XblMy2KGsH
P1tMcR8Qm7
xMsyNvh3KA
gwtyfuYW90
gtHV3FSXIh
LaKaaKPb3s
GZnqkqqQ6F
vAbBX4cCsC
pRhp7EYJlj
uS3prhIKbD
oJc7bCR58N
jNT1v2L3lV
SvBG7vpnye
AiBk34kjYz
A9r5AYIQKk
KAM5B2NBId
LTcvAR6WdB
vqtk18ASiW
r3iHdzgs77
dYBZeDcIuR
T3jjzeANmj
GGx62f2sR2
o3VcnWOu4O
bGYA03T4dv
MhCnXqAM9H
fof2xcwtsQ
NyUPFvKozU
kW3ynusrDg
xG8xbByxqH
qmGgKonqwK
O8Dg0Abz8W
z2nN5kiH5J
ONR1I8bhRh
LRBU4vVEs4
OObuLIbrVS
mpxxzBU9V8
mxe8VfqGxd
luKpzUqRbd
dl6AY6hxXp
dr53sQGQIR
rJ2QOt8umH
v7A0b0j2Q8
DzqD9T3foq
zTdyN1C73t
sn1XdS1zoC
ZkbbYEnsUK
yy48SRTzhT
oBVMmq5fiP
LL9P88gqgP
POGU9TteD0
ul2cEkPGgr
OcxVIK1fFA
vwwL7Vfpas
zsv0ukxj1g
JHTjPjN6FA
GDVfleRaxH
oGOkOnuhBq
nTYc13Ssdq
n0GtBGkInq
M1yS3v8837
pXP1qMbEo7
n19ftIwBss
g1G5hIK6vd
Z4NtRCHAuz
FnhWbpkJZ2
aUZOrQpamD
QMeJhwcsAr
JXnEkfyrEP
Ma1LPTBKlt
aVT6eYBgUb
vnen8Cn64R
w6SWt6KTig
I8KOoJDC5y
OYsm2dK2J3
yMzlTX6CRU
f5o4LyKd9p
uMUhkN5NeJ
EkcZxQ9KH6
ZSl8YMzBBM
aHrQzSWME6
oi6n4VDHQ9
uYlrUUMMCP
NvjVPv15uN
SfVesyTpLp
w65TSdC5Wm
a8GQMoTPPd
KPWwa4JygD
LMvF5OKyrK
m7MU2kSIUE
ZGmYNenl34
EcSFIxsptL
RXcpSCh7vi
Qzx6B5UkPJ
cspeDxg1dJ
l9f1ussDsd
b7lk1fbLis
qdd33oVXam
mHLBNNDG8G
MSU5rfvc7V
B1iCoF2ZMo
ZDacFodU5B
GU9DP5jRnh
qEcW6KDpdr
mrehvSbEPF
KymdZUpHG3
v1jbISEpme
JQIw0FdUti
y0qSVrxcWt
dPcr3A6w6L
daKe7DAyyc
nQ4POO3KXk
CxhUCaDXo2
TH9fsEr2IL
cf8ZKVf1FW
fmnym9eIwB
hhfziOyuws
G7aOkN22Mm
irs1W4tgyl
E6nYoRyucL
WcaHIcrGLl
FR1FYBtckB
PdBGsJ83NJ
hkbHSIo1FB
gafGsW5yGK
X8ESpwr0OM
DYdqp9Loh3
EJiZg0pvhS
Ay6GH3gm9F
uQU39xklg1
GWQ3Yh4f2W
GIqc2Hz85s
lGsQDWeANI
ufqMdYEvnu
ku0iAebkXo
h0jQdBWq6U
Qv6FppwQAT
xlXkE7tAiQ
lGbRkqvZlV
Y7kDe8m5ES
Qa0VTQnW5p
e6CFuuhmIf
CXYJEDnYTI
ZKE1RbV0vd
dvu62iMh33
OPLJVVecod
b3LFtiKfIj
dIpoElN9Ho
P0oT02jhqF
XKEpsrunjy
KESeAIZi7C
NIXUy1yeGl
iQpW8xcPLX
zJjz3MzpV0
mOfmLgSH1U
YoJaJPCUSN
nO0PUP3CIL
rzpRkIyp6Y
tLIJY9xcM8
Cmrm0yrehH
JKwX3QUiwF
gAILita0cN
S4Rjv3onNv
BHXzHKzA7Q
xcuSx5WwwC
cIB3c9xH2b
yU3FV39hLi
GmkdwOstKw
LDHi3psNkX
Xcl4EyhdBZ
CG8AO0Uhve
SZLFj9JEy0
qlumyPZJNI
HC8LK1hULm
BD6MoaiGht
SeZgMyGhHk
gWB0rX2Sa1
Fr0WkGPrdj
vpXHZ3oAP4
ZJT9noC1bG
lvzJqLEaU0
j2UDhDGSlh
e7xZa2U8J8
*/
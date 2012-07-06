unit tmd2obj;
interface
uses
Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms, Dialogs, StdCtrls, ExtCtrls, Buttons, XPMan, Spin;
type
TTmdHeader = record
Id: DWORD;
Flags: DWORD;
Object_Count: DWORD;
end;
type
TObjectHeader = record
Vertex_Offset: DWORD;
Vertex_Count: DWORD;
Normal_Offset: DWORD;
Normal_Count: DWORD;
Primitive_Offset: DWORD;
Primitive_Count: DWORD;
Scale: DWORD;
end;
type
TPrimiive3Side = record
Id: DWORD;
U0: Byte;
V0: Byte;
CBA: Word;
U1: Byte;
V1: Byte;
TSB: Word;
U2: Byte;
V2: Byte;
Pad: Word;
Nr0: Word;
Vr0: Word;
Nr1: Word;
Vr1: Word;
Nr2: Word;
Vr2: Word;
end;
type
TVertex = record
X: Word;
Y: Word;
Z: Word;
P: Word;
end;
type
TNormal = record
X: Word;
Y: Word;
Z: Word;
P: Word;
end;
procedure Tmd2ObjSingle(OBJFileName: string; ObjectNumber: Integer);
procedure Tmd2ObjAll(OBJFileName: string; Pos, Smooth: Boolean);
implementation
uses main;
const
Scale = 255;
{------------------------------------------------------------------------------}
{Calculate_Pos                                                                    }
{------------------------------------------------------------------------------}
function Cp(Pos: Word): Double;
begin
if Pos > 65535 / 2 then Result := 65535 - Pos else Result := Pos - (Pos * 2);
end;
{------------------------------------------------------------------------------}
{Tmd To Obj Single                                                             }
{------------------------------------------------------------------------------}
procedure Tmd2ObjSingle(OBJFileName: string; ObjectNumber: Integer);
var
TmdHeader: TTmdHeader;
ObjectHeader: array[0..100] of TObjectHeader;
Primiive3Side: array[0..50, 0..200] of TPrimiive3Side;
Vertex: array[0..50, 0..200] of TVertex;
Normal: array[0..50, 0..200] of TNormal;
i, ii: Integer;
s: string;
Obj_File: Textfile;
Rp: Double;
begin
Tmd_Stream.Position := 0;
{---Tmd Header---}
Tmd_Stream.Read(TmdHeader, 12);
{---Object Header---}
Tmd_Stream.Read(ObjectHeader, TmdHeader.Object_Count * 28);
{---Create Obj File---}
AssignFile(Obj_File, OBJFileName);
Rewrite(Obj_File);
Writeln(Obj_File, '# Wavefront OBJ exported by EMDViewer');
Writeln(Obj_File, '#');
for i := ObjectNumber to ObjectNumber do
begin
{---Primitive---}
Tmd_Stream.Position := ObjectHeader[i].Primitive_Offset + 12;
for ii := 0 to ObjectHeader[i].Primitive_Count - 1 do Tmd_Stream.Read(Primiive3Side[i, ii], 28);
{---Vertex---}
Tmd_Stream.Position := ObjectHeader[i].Vertex_Offset + 12;
for ii := 0 to ObjectHeader[i].Vertex_Count - 1 do Tmd_Stream.Read(Vertex[i, ii], 8);
{---Normal---}
Tmd_Stream.Position := ObjectHeader[i].Normal_Offset + 12;
for ii := 0 to ObjectHeader[i].Normal_Count - 1 do Tmd_Stream.Read(Normal[i, ii], 8);
ObjectNumber := i;
Writeln(Obj_File, '# object Object_' + IntToStr(i) + ' to come ...');
Writeln(Obj_File, '#');
{---Write Vertex---}
for ii := 0 to ObjectHeader[ObjectNumber].Vertex_Count - 1 do
begin
s := 'v  ' + FormatFloat('#,######0.000000', (Cp(Vertex[ObjectNumber, ii].Z)) / 50)
+ ' ' + FormatFloat('#,######0.000000', (Cp(Vertex[ObjectNumber, ii].Y)) / 50)
+ ' ' + FormatFloat('#,######0.000000', (Cp(Vertex[ObjectNumber, ii].X)) / 50);
Writeln(Obj_File, s);
end;
Writeln(Obj_File, '# ' + IntToStr(ObjectHeader[ObjectNumber].Vertex_Count) + ' vertices');
Writeln(Obj_File, '');
{---Write Texture UV---}
for ii := 0 to ObjectHeader[ObjectNumber].Primitive_Count - 1 do
begin
Rp := (Primiive3Side[ObjectNumber, ii].TSB and $1F) * 0.5;
s := 'vt  ' + FormatFloat('#,######0.000000', (Primiive3Side[ObjectNumber, ii].U2 / Scale) + Rp) + ' ' + FormatFloat('#,######0.000000', -(Primiive3Side[ObjectNumber, ii].V2 / Scale) +1);
Writeln(Obj_File, s);
s := 'vt  ' + FormatFloat('#,######0.000000', (Primiive3Side[ObjectNumber, ii].U1 / Scale) + Rp) + ' ' + FormatFloat('#,######0.000000', -(Primiive3Side[ObjectNumber, ii].V1 / Scale) +1);
Writeln(Obj_File, s);
s := 'vt  ' + FormatFloat('#,######0.000000', (Primiive3Side[ObjectNumber, ii].U0 / Scale) + Rp) + ' ' + FormatFloat('#,######0.000000', -(Primiive3Side[ObjectNumber, ii].V0 / Scale) +1);
Writeln(Obj_File, s);
end;
Writeln(Obj_File, '# ' + IntToStr(ObjectHeader[ObjectNumber].Primitive_Count * 3) + ' texture coordinates');
Writeln(Obj_File, '');
{---Write Normal---}
for ii := 0 to ObjectHeader[ObjectNumber].Normal_Count - 1 do
begin
s := 'vn  ' + FormatFloat('#,######0.000000', (Cp(Normal[ObjectNumber, ii].Z)) / 3267)
+ ' ' + FormatFloat('#,######0.000000', (Cp(Normal[ObjectNumber, ii].Y)) / 3267)
+ ' ' + FormatFloat('#,######0.000000', (Cp(Normal[ObjectNumber, ii].X)) / 3267);
Writeln(Obj_File, s);
end;
Writeln(Obj_File, '# ' + IntToStr(ObjectHeader[ObjectNumber].Normal_Count) + ' vertex normals');
Writeln(Obj_File, '');
Writeln(Obj_File, 'g Object_' + IntToStr(i));
{---Write Face---}
for ii := 0 to ObjectHeader[ObjectNumber].Primitive_Count - 1 do
begin
s := 'f ' + IntToStr(Primiive3Side[ObjectNumber, ii].Vr2 + 1) + '/' + IntToStr(((ii * 3) + 1)) + '/' + IntToStr(Primiive3Side[ObjectNumber, ii].Nr0 + 1)
+ ' ' + IntToStr(Primiive3Side[ObjectNumber, ii].Vr1 + 1) + '/' + IntToStr(((ii * 3) + 2)) + '/' + IntToStr(Primiive3Side[ObjectNumber, ii].Nr1 + 1)
+ ' ' + IntToStr(Primiive3Side[ObjectNumber, ii].Vr0 + 1) + '/' + IntToStr(((ii * 3) + 3)) + '/' + IntToStr(Primiive3Side[ObjectNumber, ii].Nr2 + 1) + ' ';
Writeln(Obj_File, s);
end;
Writeln(Obj_File, '# ' + IntToStr(ObjectHeader[ObjectNumber].Primitive_Count) + ' faces');
Writeln(Obj_File, '');
Writeln(Obj_File, 'g');
end;
{---Close Obj File---}
CloseFile(Obj_File);
end;
{------------------------------------------------------------------------------}
{Tmd To Obj All                                                                }
{------------------------------------------------------------------------------}
procedure Tmd2ObjAll(OBJFileName: string; Pos, Smooth: Boolean);
const
Scale = 255;
Pres = '#,###0.000';
var
TmdHeader: TTmdHeader;
ObjectHeader: array[0..100] of TObjectHeader;
Pr: array[0..50, 0..200] of TPrimiive3Side;
Vertex: array[0..50, 0..200] of TVertex;
Normal: array[0..50, 0..200] of TNormal;
i, ii, Vc, Tc, Nc: Integer;
s: string;
Obj_File: Textfile;
Rp: Double;
begin
Tmd_Stream.Position := $0;
{---Tmd Header---}
Tmd_Stream.Read(TmdHeader, 12);
{---Object Header---}
Tmd_Stream.Read(ObjectHeader, TmdHeader.Object_Count * 28);
{---Read TMD---}
for i := 0 to TmdHeader.Object_Count -1 do
begin
{---Primitive---}
Tmd_Stream.Position := ObjectHeader[i].Primitive_Offset + 12;
for ii := 0 to ObjectHeader[i].Primitive_Count -1 do Tmd_Stream.Read(Pr[i, ii], 28);
{---Vertex---}
Tmd_Stream.Position := ObjectHeader[i].Vertex_Offset + 12;
for ii := 0 to ObjectHeader[i].Vertex_Count -1 do Tmd_Stream.Read(Vertex[i, ii], 8);
{---Normal---}
Tmd_Stream.Position := ObjectHeader[i].Normal_Offset + 12;
for ii := 0 to ObjectHeader[i].Normal_Count -1 do Tmd_Stream.Read(Normal[i, ii], 8);
end;
{---Create Obj File---}
AssignFile(Obj_File, OBJFileName);
Rewrite(Obj_File);
Writeln(Obj_File, '# Wavefront OBJ exported by EMDViewer');
Writeln(Obj_File, '#');
{---Write OBJ---}
for i := 0 to TmdHeader.Object_Count -1 do
begin
Writeln(Obj_File, '# object Object_' + IntToStr(i) + ' to come ...');
Writeln(Obj_File, '#');
{---Write Vertex---}
for ii := 0 to ObjectHeader[i].Vertex_Count -1 do
begin
if Pos then
begin
s := 'v'
+ ' ' + FormatFloat(Pres, (Cp(Vertex[i, ii].Z) +Obj_Pos[i, 0] *100) / 50)
+ ' ' + FormatFloat(Pres, (Cp(Vertex[i, ii].Y) +Obj_Pos[i, 1] *100) / 50)
+ ' ' + FormatFloat(Pres, (Cp(Vertex[i, ii].X) +Obj_Pos[i, 2] *100) / 50);
end else
begin
s := 'v'
+ ' ' + FormatFloat(Pres, (Cp(Vertex[i, ii].Z)) / 50)
+ ' ' + FormatFloat(Pres, (Cp(Vertex[i, ii].Y)) / 50)
+ ' ' + FormatFloat(Pres, (Cp(Vertex[i, ii].X)) / 50);
end;
Writeln(Obj_File, s);
end;
Writeln(Obj_File, '# ' + IntToStr(ObjectHeader[i].Vertex_Count) + ' vertices');
Writeln(Obj_File, '');
{---Write Texture UV---}
for ii := 0 to ObjectHeader[i].Primitive_Count -1 do
begin
Rp := (Pr[i, ii].TSB and $1F) * 0.5;
s := 'vt  ' + FormatFloat(Pres, (Pr[i, ii].U2 / Scale) + Rp) + ' ' + FormatFloat(Pres, -(Pr[i, ii].V2 / Scale) +1);
Writeln(Obj_File, s);
s := 'vt  ' + FormatFloat(Pres, (Pr[i, ii].U1 / Scale) + Rp) + ' ' + FormatFloat(Pres, -(Pr[i, ii].V1 / Scale) +1);
Writeln(Obj_File, s);
s := 'vt  ' + FormatFloat(Pres, (Pr[i, ii].U0 / Scale) + Rp) + ' ' + FormatFloat(Pres, -(Pr[i, ii].V0 / Scale) +1);
Writeln(Obj_File, s);
end;
Writeln(Obj_File, '# ' + IntToStr(ObjectHeader[i].Primitive_Count * 3) + ' texture coordinates');
Writeln(Obj_File, '');
{---Write Normal---}
for ii := 0 to ObjectHeader[i].Normal_Count -1 do
begin
s := 'vn '
+ ' ' + FormatFloat(Pres, (Cp(Normal[i, ii].Z)) / 3267)
+ ' ' + FormatFloat(Pres, (Cp(Normal[i, ii].Y)) / 3267)
+ ' ' + FormatFloat(Pres, (Cp(Normal[i, ii].X)) / 3267);
Writeln(Obj_File, s);
end;
Writeln(Obj_File, '# ' + IntToStr(ObjectHeader[i].Normal_Count) + ' vertex normals');
Writeln(Obj_File, '');
Writeln(Obj_File, 'g Object_' + IntToStr(i));
if Smooth then Writeln(Obj_File, 's 1');
{---Write Face---}
for ii := 0 to ObjectHeader[i].Primitive_Count -1 do
begin
s := 'f'
+ ' ' + IntToStr(Pr[i, ii].Vr2 + Vc + 1) + '/' + IntToStr(((ii * 3) + Tc + 1)) + '/' + IntToStr(Pr[i, ii].Nr0 + Nc + 1)
+ ' ' + IntToStr(Pr[i, ii].Vr1 + Vc + 1) + '/' + IntToStr(((ii * 3) + Tc + 2)) + '/' + IntToStr(Pr[i, ii].Nr1 + Nc + 1)
+ ' ' + IntToStr(Pr[i, ii].Vr0 + Vc + 1) + '/' + IntToStr(((ii * 3) + Tc + 3)) + '/' + IntToStr(Pr[i, ii].Nr2 + Nc + 1) + ' ';
Writeln(Obj_File, s);
end;
{---Inc Counter---}
Vc := Vc + ObjectHeader[i].Vertex_Count;
Tc := Tc + ObjectHeader[i].Primitive_Count *3;
Nc := Nc + ObjectHeader[i].Normal_Count;
Writeln(Obj_File, '# ' + IntToStr(ObjectHeader[i].Primitive_Count) + ' faces');
Writeln(Obj_File, '');
Writeln(Obj_File, 'g');
end;
{---Close Obj File---}
CloseFile(Obj_File);
end;
end.


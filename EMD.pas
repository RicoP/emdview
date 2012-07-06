Obj_Pos : Array [0..100, 0..2] of GLfloat;
...
...
...
{------------------------------------------------------------------------------}
{Load Emd                                                                      }
{------------------------------------------------------------------------------}
procedure TMain_Form.Load_Emd(FileName: string);
const
File_Ext : Array[0..5] of String = ('.EMR', '.EDD', '.EMR', '.EDD', '.TMD', '.TIM');
var
Emd, Tmd, Emr, Tim : TMemoryStream;
File_Offset : Array [0..6] of DWORD;
i : Integer;
File_Name : String;
Temp_Dw : Dword;
begin
File_Name := ChangeFileExt(ExtractFileName(FileName), '');
Emd := TMemoryStream.Create;
Tmd := TMemoryStream.Create;
Emr := TMemoryStream.Create;
Tim := TMemoryStream.Create;
Emd.LoadFromFile(FileName);
Emd.Position := Emd.Size -20;
for i := 1 to 5 do Emd.Read(File_Offset[i], 4); File_Offset[6] := Emd.Size -20;
{---Load Tmd---}
Emd.Position := File_Offset[4];
Tmd.CopyFrom(Emd, File_Offset[5] -File_Offset[4]);
Load_Tmd(Tmd);
{---Load Emr---}
Emd.Position := File_Offset[2];
Emr.CopyFrom(Emd, File_Offset[3] -File_Offset[2]);
Load_Emr(Emr);
{---Load Tim---}
Emd.Position := File_Offset[5];
Tim.CopyFrom(Emd, File_Offset[6] -File_Offset[5]);
{---Export Tim---}
if Assigned(Tim_Export) then Tim_Export.Clear;
Emd.Position := File_Offset[5];
Tim_Export.CopyFrom(Emd, File_Offset[6] -File_Offset[5]);
{---Export Tmd---}
if Assigned(Tmd_Export) then Tmd_Export.Clear;
Emd.Position := File_Offset[4];
Tmd_Export.CopyFrom(Emd, File_Offset[5] -File_Offset[4]);
{------}
Load_Tim(Tim);
Make_Object();
Make_Grid(20);
Tim_Offset := File_Offset[5];
Tmd_Offset := File_Offset[4];
Emd.Free;
Make_Texture();
end;
{------------------------------------------------------------------------------}
{Calculate Position                                                            }
{------------------------------------------------------------------------------}
function Calculate_Pos(Pos: Word): GLfloat;
begin
if Pos > 65535 / 2 then Result := (65535 - Pos) /100 else Result := (Pos - (Pos * 2)) /100;
end;
{------------------------------------------------------------------------------}
{Load Emr                                                                      }
{------------------------------------------------------------------------------}
procedure TMain_Form.Load_Emr(f : TMemoryStream);
var
b : Byte;
w, ww, p, c : Word;
i, ii : Integer;
begin
f.Position := $0;
f.Read(p, 2);
f.Position := $4;
f.Read(c, 2);
f.Position := $8;
for i := 0 to TmdHeader[0].Object_Count -1 do
begin
f.Read(w, 2); Obj_Pos[i, 2] := Calculate_Pos(w);
f.Read(w, 2); Obj_Pos[i, 1] := Calculate_Pos(w);
f.Read(w, 2); Obj_Pos[i, 0] := Calculate_Pos(w);
end;
f.Position := f.Position +2;
for ii := 0 to c -1 do
begin
f.Position := p + (4 *ii);
f.Read(w, 2);
f.Read(ww, 2);
f.Position := p +(ww);
for i := 0 to w -1 do
begin
f.Read(b, 1);
Obj_Pos[b, 0] := Obj_Pos[b, 0] + Obj_Pos[ii, 0];
Obj_Pos[b, 1] := Obj_Pos[b, 1] + Obj_Pos[ii, 1];
Obj_Pos[b, 2] := Obj_Pos[b, 2] + Obj_Pos[ii, 2];
end;
end;
f.Free;
end;

# coding: UTF-8
"""
GSTOS関連処理
"""

import os

OUTPUT_DIR_GSTOS  = "gstos_files"

def GenerateGstosFiles(settings, sgc_db, tlm_db):
    if not os.path.exists(OUTPUT_DIR_GSTOS):
        os.makedirs(OUTPUT_DIR_GSTOS)

    cmd_sib_file_path = r"./" + OUTPUT_DIR_GSTOS + r"/cmdfile.cmd.sib"
    tlm_sib_file_path = r"./" + OUTPUT_DIR_GSTOS + r"/tlmfile.tlm.sib"

    output_cmd = ""
    output_tlm = ""

    output_cmd += '<?xml version="1.0" encoding="EUC-JP"?>\n'
    output_cmd += '<cmdfile>\n'

    DATA_SART_ROW = 3
    for i in range(DATA_SART_ROW, len(sgc_db)):
        comment = sgc_db[i][0]
        name    = sgc_db[i][1]
        if comment == "" and name == "":                  # CommentもNameも空白なら打ち切り
            break
        if comment != "":                                 # Comment
            continue
        output_cmd += GetCmdSibDef_(sgc_db[i])

    output_cmd += '</cmdfile>\n'

    with open(cmd_sib_file_path, mode='w', encoding='euc_jp') as fh:
        fh.write(output_cmd)

    output_tlm += '<?xml version="1.0" encoding="EUC-JP"?>\n'
    output_tlm += '<tlmfile>\n'

    DATA_SART_ROW = 7

    for tlm in tlm_db:
        for i in range(DATA_SART_ROW, len(tlm['data'])):
            comment  = tlm['data'][i][0]
            name     = tlm['data'][i][1]
            var_type = tlm['data'][i][2]
            # code     = tlm['data'][i][3]
            # pos      = tlm['data'][i][4]
            if comment == "" and name == "":                    # CommentもNameも空白なら打ち切り
                break
            if comment != "":
                continue
            if name == "":
                continue
            if var_type == "":
                continue
            # if code == "":
            #     continue
            # if pos == "":
            #     continue
            output_tlm += GetTlmSibDef_(tlm['tlm_id'], tlm['data'][i])

    output_tlm += '</tlmfile>\n'

    with open(tlm_sib_file_path, mode='w', encoding='euc_jp') as fh:
        fh.write(output_tlm)


def GetCmdSibDef_(sgc_db_line):
    cmd = sgc_db_line[1]
    inf = sgc_db_line[18]
    cmd_id = sgc_db_line[3]
    param_num = int(sgc_db_line[4])
    START_PARAM_COL = 5
    is_danger = sgc_db_line[17]

    output = ""

    for cmd_type in range(0,3):
        # 0,1,2 -> RT,TL,BL
        output += '  <def>\n'
        if cmd_type == 0:
            output += '   <cmd>OBC_RT.' + cmd + ' </cmd>\n'
        elif cmd_type == 1:
            output += '   <cmd>OBC_BL.' + cmd + ' </cmd>\n'
        else:
            output += '   <cmd>OBC_TL.' + cmd + ' </cmd>\n'
        output += '   <inf>' + inf + ' </inf>\n'
        output += '   <cmdack>UNUSE</cmdack>\n'
        output += '   <apid>0x0210</apid>\n'
        output += '   <val>432</val>\n'
        if is_danger:
            output += '    <opt>K</opt>\n'
        output += '   <code>\n'
        output += '       <!-- Primary Header  -->\n'
        output += '       <const>o0,b0@16=0x1a10 </const> <!-- Oct 0,1: Version Number & Type & Application Process ID (Fix)  -->\n'
        output += '       <const>o2,b0@16=0xC000 </const> <!-- Oct 2,3: Sequence Flag & Sequence Count (Fix)  -->\n'
        output += '       <!-- Secondary Header  -->\n'
        output += '       <const>o6,b0@8=0x01 </const>    <!-- Oct 6: Format ID (Fix)  -->\n'
        output += '       <!-- User Data : Header  -->\n'
        output += '       <const>o7,b0@8=0x02 </const>    <!-- Oct 7: Command Type (Fix)  -->\n'
        output += '       <const>o8,b0@16=' + cmd_id + '</const>\n'
        if cmd_type == 0:
            output += '       <const>o10,b0@8=0x00</const>\n'
            output += '       <const>o11,b0@32=0x00000000 </const>    <!-- Oct 11,12,13,14: Time Indicator = 0x00000000: Dummy (Fix)  -->\n'
        elif cmd_type == 1:
            output += '       <const>o10,b0@8=0x01</const>\n'
            output += '       <!-- Time Definition  -->\n'
            output += '       <paramdef>\n'
            output += '           <pos>o11,b0@32 </pos>\n'
            output += '           <paraminf>Time(UINT,32BIT)</paraminf>\n'
            output += '           <rawdatatype>UINT</rawdatatype>\n'
            output += '       </paramdef>\n'
        else:
            output += '       <const>o10,b0@8=0x02</const>\n'
            output += '       <!-- Time Definition  -->\n'
            output += '       <paramdef>\n'
            output += '           <pos>o11,b0@32 </pos>\n'
            output += '           <paraminf>Time(UINT,32BIT)</paraminf>\n'
            output += '           <rawdatatype>UINT</rawdatatype>\n'
            output += '       </paramdef>\n'

        if param_num:
            o = 0
            l = 0
            t = 0
            b = 0
            for s in range(0, param_num):           # パラメーター数に制限はなし．シートで定義したぶんだけ生成可能
                str_info = "param" + str(s + 1);       # いくつ目のパラメーターか表示
                var_type = sgc_db_line[START_PARAM_COL + s * 2]
                var_note = ""

                if   var_type == "int8_t":
                    var_note = "SINT"
                    o = 15 + t
                    l = 8
                    t += 1
                elif var_type == "int16_t":
                    var_note = "SINT"
                    o = 15 + t
                    l = 16
                    t += 2
                elif var_type == "int32_t":
                    var_note = "SINT"
                    o = 15 + t
                    l = 32
                    t += 4
                elif var_type == "uint8_t":
                    var_note = "UINT"
                    o = 15 + t
                    l = 8
                    t += 1
                elif var_type == "uint16_t":
                    var_note = "UINT"
                    o = 15 + t
                    l = 16
                    t += 2
                elif var_type == "uint32_t":
                    var_note = "UINT"
                    o = 15 + t
                    l = 32
                    t += 4
                elif var_type == "float":
                    var_note = "FLOAT"
                    o = 15 + t
                    l = 32
                    t += 4
                elif var_type == "double":
                    var_note = "DOUBLE"
                    o = 15 + t
                    l = 64
                    t += 8
                elif var_type == "raw":
                    var_note = "HEXBINARY"
                    o = 15 + t
                    l = (432 - o) * 8
                    # t += 4        # コマンド長最後まで
                else:
                    print("Error: Cmd DB Err at " + cmd, file=sys.stderr)
                    sys.exit(1)

                output += '        <paramdef>\n'
                output += '            <pos>o' + str(o) + ',b' + str(b) + '@' + str(l) + '</pos>\n'
                output += '            <paraminf>' + str_info + ' (' + var_type + ',' + str(l) + 'BIT)</paraminf>\n'
                output += '            <rawdatatype>' + var_note + '</rawdatatype>\n'
                output += '        </paramdef>\n'

        output += '   </code>\n'
        output += '  </def>\n'

    return output


def GetTlmSibDef_(tlm_id, tlm_db_line):
    comment  = tlm_db_line[0]
    name     = tlm_db_line[1]
    var_type = tlm_db_line[2]
    code     = tlm_db_line[3]
    pos      = tlm_db_line[4]
    ext_type = tlm_db_line[5]
    oct_pos  = tlm_db_line[6]
    bit_pos  = tlm_db_line[7]
    bit_len  = tlm_db_line[8]
    cnv_type = tlm_db_line[9]
    status   = tlm_db_line[16]
    var_note = ""

    tlm_id = tlm_id.replace("0x", "")

    if   var_type == "int8_t":
        var_note = "SINT"
    elif var_type == "int16_t":
        var_note = "SINT"
    elif var_type == "int32_t":
        var_note = "SINT"
    elif var_type == "uint8_t":
        var_note = "UINT"
    elif var_type == "uint16_t":
        var_note = "UINT"
    elif var_type == "uint32_t":
        var_note = "UINT"
    elif var_type == "float":
        var_note = "FLOAT"
    elif var_type == "double":
        var_note = "DOUBLE"
    else:
        print("Error: Tlm DB Err at " + name, file=sys.stderr)
        sys.exit(1)

    # 今回はめんどいのでPACKETしかみない
    if ext_type != "PACKET":
        print("Error: Tlm DB Err at " + name, file=sys.stderr)
        sys.exit(1)

    output = ""

    output += '  <def>\n'
    output += '    <tlm>' + name + ' </tlm>\n'
    output += '    <rawdatatype>' + var_note + ' </rawdatatype>\n'
    output += '    <ext>p510:s' + tlm_id + ',o' + oct_pos + ',b' + bit_pos + '@' + bit_len + ' </ext>\n'

    if   cnv_type == "POLY":
        coeffs = []
        for k in range(10,16):
            coeff = tlm_db_line[k]
            if coeff == "":
                break
            coeffs.append(coeff)
        output += '    <pol>' + ",".join(coeffs) + '</pol>\n'
    elif cnv_type == "STATUS":
        output += '    <sts>' + status + '</sts>\n'
    elif cnv_type == "NONE":
        output += '    <nocvt></nocvt>\n'
    else:
        print("Error: Tlm DB Err at " + name, file=sys.stderr)
        sys.exit(1)

    output += '  </def>\n'

    return output

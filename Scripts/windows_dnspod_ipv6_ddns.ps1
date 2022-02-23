
$ipv6_last = ""

while ($true)
{

    $ipconfig_body = ipconfig
    $ipv6_line = ""
    $idx = 0

    while ($true)
    {
        if ($ipconfig_body[$idx].IndexOf("IPv6") -ge 0)
        {
            $ipv6_line = $ipconfig_body[$idx]
            break
        }
        $idx = $idx + 1
    }

    $ipv6 = $ipv6_line.Substring($ipv6_line.IndexOf("2"))
    if ($ipv6 -ne $ipv6_last)
    {
        $param = ""
        $param += "login_token=" + "293168" + "," + "417de0a48e1a12448864c5f8a34c965e" + "&"
        $param += "format=json&"
        $param += "domain=" + "pvl9.net" + "&"
        $param += "record_id=" + "1062086283" + "&"
        $param += "record_line_id=0" + "&"
        $param += "sub_domain=@" + "&"
        $param += "value=" + $ipv6 + "&"
        $param += "record_type=" + "AAAA" + "&"

        curl.exe -s -X POST https://dnsapi.cn/Record.Modify -d $param

        $ipv6_last = $ipv6
        echo "ipv6 change to $ipv6"
    }
    else
    {
        echo "ipv6 not change"
    }

    Sleep 600
}

exit

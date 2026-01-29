$a=(Get-NetAdapter -Physical | Where-Object { $_.Status -eq 'Up' })[0]
Set-NetAdapter $a -MacAddress "BC2411B9B662" -Confirm:$false
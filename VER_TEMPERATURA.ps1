# Terminal opcional de solo lectura, 115200 8N1, sin control de flujo.
$ErrorActionPreference = 'Stop'
Write-Host 'Puertos disponibles:'
[System.IO.Ports.SerialPort]::GetPortNames() | Sort-Object | ForEach-Object { Write-Host $_ }
$temperaturePortName = (Read-Host 'COM de ST-LINK, por ejemplo COM5').Trim()
if ($temperaturePortName -notmatch '^COM\d+$') { throw 'Nombre de puerto no valido.' }
$temperaturePort = [System.IO.Ports.SerialPort]::new($temperaturePortName,115200,[System.IO.Ports.Parity]::None,8,[System.IO.Ports.StopBits]::One)
$temperaturePort.Handshake = [System.IO.Ports.Handshake]::None
$temperaturePort.ReadTimeout = 500
try {
    $temperaturePort.Open()
    Write-Host 'Esperando datos de la placa. Ctrl+C para salir.'
    while ($true) {
        try { Write-Host ($temperaturePort.ReadLine().TrimEnd("`r")) }
        catch [System.TimeoutException] { }
    }
} finally {
    if ($temperaturePort.IsOpen) { $temperaturePort.Close() }
    $temperaturePort.Dispose()
}

param( 
    [string]$to,
    [string]$subject,
    [string]$body
)

$SMTPServer = "smtp.gmail.com"
$SMTPPort = "587"
$From = "your-email@gmail.com"
$Username = "egdeepakeg@gmail.com"
$Password = "bfrv hyqv grsw zbmo"

# Convert password to a secure string
$SecurePassword = ConvertTo-SecureString $Password -AsPlainText -Force
$Credential = New-Object System.Management.Automation.PSCredential ($Username, $SecurePassword)

# Replace `n with actual newline characters
$body = $body -replace "`n", [System.Environment]::NewLine

# Send the email
Send-MailMessage -From $From -To $to -Subject $subject -Body $body -SmtpServer $SMTPServer -Credential $Credential -UseSsl -Port $SMTPPort

Write-Output "------ Email sended successfully!"
Write-Output "To: $to"
Write-Output "Subject: $subject"
Write-Output "Body: $body"

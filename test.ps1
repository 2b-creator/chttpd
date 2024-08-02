# URL of the API endpoint
$url = "http://127.0.0.1:8888/api/user/send"

# Data to send in the POST request
$body = @{
    report_type = 1
    description = "中文测试"
}

# Convert the data to JSON format
$jsonBody = $body | ConvertTo-Json

# Headers for the request
$headers = @{
    "Content-Type" = "application/json"
    "Authorization" = "1e661c99-3741-4dde-92b4-b1eb6ea716fb"
}

# Send the POST request
$response = Invoke-RestMethod -Uri $url -Method Post -Body $jsonBody -Headers $headers

# Output the response
$response

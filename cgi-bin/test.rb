#!/usr/bin/ruby

# Read input from stdin
input_data = gets.chomp

# Parse the input (e.g., name=John)
data = input_data.split('&').each_with_object({}) do |pair, hash|
    key, value = pair.split('=')
    hash[key] = value
end
name = data['name'] || 'Guest'

# Print the response
puts "Content-Type: text/html\n\n"
puts "<h1>Hello, #{name}!</h1>"
puts "<p>This is a Ruby CGI script.</p>"
<?php
require 'vendor/autoload.php';
include_once 'walkthrough/connection.inc';
include_once 'walkthrough/authenticator/basic.inc';
include_once 'walkthrough/commands.inc';

$command_line = new Commando\Command();

$command_line->option('username')
  ->aka('u')
  ->describedAs('Drupal user name')
  ->require();

$command_line->option('password')
  ->aka('p')
  ->describedAs('Drupal password')
  ->require();

$command_line->option('walkhub_url')
  ->aka('w')
  ->describedAs('Walkhub url.')
  ->require();

$command_line->argument()
  ->referToAs('Action')
  ->describe('Action to perform. Possible options:
  * status
    Test connection to the walkhub.

  * get_queue
    Get the screenshot queue.

  * get_phpunit [uuid]
    Get the pphunit export for a walkthrough.

  * process_queue
    Gets the first item of the queue and executes the phpunit test, when ready
    posts back the results and the screenshots to the screening.');

$command_line->option('debug')
  ->aka('d')
  ->describedAs('Debug mode')
  ->boolean();

$function_name = 'command__' . $command_line[0];
if (function_exists($function_name)) {
  $endpoint = $command_line['walkhub_url'] . '/api/v2';
  $connection = new Walkthrough\Connection();
  $connection->setEndpoint($endpoint);

  // Basic http authentication.
  $authenticator = new Walkthrough\Authenticator\Basic();
  $authenticator->setUsername($command_line['username']);
  $authenticator->setPassword($command_line['password']);
  $authenticator->setEndpoint($endpoint);

  $connection->setAuthenticator($authenticator);

  call_user_func($function_name, $connection, $command_line);
}

function format_error(Guzzle\Http\Message\Response $response) {
  return 'Error: [' . $response->getStatusCode() . '] ' . $response->getReasonPhrase() . "\n";
}
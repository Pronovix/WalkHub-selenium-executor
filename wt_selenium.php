<?php

namespace Walkthrough;

use Walkthrough\Authenticator\NotAuthenticatedException;

require 'vendor/autoload.php';
include_once 'walkthrough/connection.inc';
include_once 'walkthrough/commands.inc';
include_once 'walkthrough/authenticator/factory.inc';


$command_line = new \Commando\Command();

$command_line->option('username')
  ->aka('u')
  ->describedAs('Drupal user name');

$command_line->option('password')
  ->aka('p')
  ->describedAs('Drupal password');

$command_line->option('walkhub_url')
  ->aka('w')
  ->describedAs('Walkhub url')
  ->require();

$command_line->option('consumer key')
  ->aka('k')
  ->describedAs('OAuth Consumer Key');

$command_line->option('consumer secret')
  ->aka('s')
  ->describedAs('OAuth Consumer Secret');

$command_line->argument()
  ->referToAs('Action')
  ->describe('Action to perform. Possible options:
  * status
    Test connection to the walkhub.

  * get_queue
    Get the screenshot queue.

  * get_phpunit [walkthrough|walkthrough_set] [uuid]
    Get the phpunit export for a walkthrough.

  * process_queue
    Gets the first item of the queue and executes the phpunit test, when ready
    posts back the results and the screenshots to the screening.

  * flag [uuid] [0|1]
    Flags/unflags a Walkthrough or Walkthrough set.');

$command_line->option('debug')
  ->aka('d')
  ->describedAs('Debug mode')
  ->boolean();

$function_name = 'command__' . $command_line[0];
if (function_exists($function_name)) {
  $connection = new Connection();

  try {
    $authenticator = Authenticator\AuthenticatorFactory::create($command_line);
  } catch (NotAuthenticatedException $e) {
    $warning_message = "Warning: Authentication not set, using anonymous session (most endpoints will not work).\n";
    $warning_message .= "  Use the -u and -p flags for basic HTTP authentication.\n";
    $warning_message .="  Use the -k and -s flags for 2-legged OAuth authentication.\n\n";
    echo $warning_message;

    // We let it continue, good for testing if we can reach endpoints
    // unauthenticated (We shouldn't...).
    include_once 'walkthrough/authenticator/anonymous.inc';
    $authenticator = new \Walkthrough\Authenticator\Anonymous();
    $authenticator->setEndpoint($command_line['walkhub_url'] . '/api/v2');
  }
  $connection->setAuthenticator($authenticator);
  $connection->setEndpoint($authenticator->getEndpoint());

  if ($command_line['debug']) {
    echo "Endpoint set to: " . $authenticator->getEndpoint() . "\n";
  }

  call_user_func($function_name, $connection, $command_line);
}

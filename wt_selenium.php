#!/usr/bin/env php
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
    Use --extend_custom_class to have the test extend your own class instead of
    PHPUnit_Extensions_Selenium2TestCase.

  * process_queue
    Gets the first item of the queue and executes the phpunit test, when ready
    posts back the results and the screenshots to the screening.
    Use --extend_custom_class to have the test extend your own class instead of
    PHPUnit_Extensions_Selenium2TestCase.
    Use --browser to override the default browser (firefox).

  * flag [uuid] [0|1]
    Flags/unflags a Walkthrough or Walkthrough set.');

$command_line->option('debug')
  ->aka('d')
  ->describedAs('Debug mode')
  ->boolean();

$command_line->option('extend_custom_class')
  ->aka('e')
  ->describedAs('Extend custom class');

$command_line->option('process_queue_length')
  ->aka('l')
  ->describedAs('Process queue length');

$command_line->option('browser')
  ->aka('b')
  ->describedAs('Run selenium via a custom browser.
If ommitted, tests will run using firefox.');

$function_name = 'command__' . $command_line[0];
if (function_exists($function_name)) {
  // Prepare authenticator object.
  try {
    $authenticator = Authenticator\AuthenticatorFactory::create($command_line);
  }
  catch (NotAuthenticatedException $e) {
    $warning_message = "Warning: Authentication not set, using anonymous session (most endpoints will not work).\n";
    $warning_message .= "  Use the -u and -p flags for basic HTTP authentication.\n";
    $warning_message .= "  Use the -k and -s flags for 2-legged OAuth authentication.\n\n";
    echo $warning_message;

    // We let it continue, good for testing if we can reach endpoints
    // unauthenticated (We shouldn't...).
    include_once 'walkthrough/authenticator/anonymous.inc';
    $authenticator = new Authenticator\Anonymous();
    $authenticator->setEndpoint($command_line['walkhub_url'] . '/api/v2');
  }

  // Prepare connection.
  $connection = new Connection();
  $connection->setAuthenticator($authenticator);
  $connection->setEndpoint($authenticator->getEndpoint());

  if ($command_line['debug']) {
    echo "Endpoint set to: " . $authenticator->getEndpoint() . "\n";
  }

  // Dispatch command.
  try {
    $return_code = call_user_func($function_name, $connection, $command_line);
    if ($return_code !== NULL) {
      exit($return_code);
    }
  }
  catch (\Guzzle\Http\Exception\ClientErrorResponseException $e) {
    echo format_error($e->getResponse());
    exit($e->getResponse()->getStatusCode());
  }
}

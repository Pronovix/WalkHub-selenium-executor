<?php
namespace Walkthrough;
use Guzzle\Http\Client;

class Connection {
  /**
   * @var \Walkthrough\Authenticator\WalkthroughAuthenticator
   * Authenticator object.
   */
  protected $authenticator;

  /**
   * @var string
   */
  protected $endpoint;

  /**
   * @var \stdClass
   * Drupal user object.
   */
  protected $drupalUser;

  public function login() {
    $this->drupalUser = $this->authenticator->login();

    return $this->drupalUser;
  }

  public function getScreeningQueue() {
    $client = $this->authenticator->getClient();
    return $client->get('walkhub-walkthrough-screening-queue')->send()->json();
  }

  /**
   * Gets the PHPUnit output for a walkthrough or a walkthrough set.
   *
   * @param $type string
   *   Entity type to render.
   *   "walkthrough" or "walkthrough_set"
   * @param $uuid string
   *   Universally unique ID.
   *
   * @return string
   *   PHPUnit script.
   */
  public function getPhpunit($type, $uuid) {
    $client = $this->authenticator->getClient();

    if ($type == 'walkthrough') {
      $response = $client->get('walkthrough-phpunit/' . $uuid)->send()->json();
    }
    else {
      $response = $client->get('walkthrough-set-phpunit/' . $uuid)->send()->json();
    }

    return $response[0];
  }

  /**
   * @param Authenticator\WalkthroughAuthenticator $authenticator
   */
  public function setAuthenticator(\Walkthrough\Authenticator\WalkthroughAuthenticator $authenticator) {
    $this->authenticator = $authenticator;
  }

  /**
   * Set the rest endpoint.
   *
   * @param string $endpoint
   */
  public function setEndpoint($endpoint) {
    $this->endpoint = $endpoint;
  }
}

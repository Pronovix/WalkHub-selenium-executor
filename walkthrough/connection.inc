<?php
namespace Walkthrough;

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
   * Update a screening flag for a Walkthrough or Walkthrough set.
   *
   * @param string $uuid
   * @param bool $flag_value
   */
  public function updateScreeningFlag($uuid, $flag_value) {
    $client = $this->authenticator->getClient();

    $response = $client->post($this->endpoint . '/user/token')->send()->json();
    $token = $response['token'];

    $client->put('walkthrough-screening-flag/'. $uuid,
      array('X-CSRF-TOKEN' => $token),
      array('flag' => $flag_value)
    )->send()->json();
  }

  /**
   * Set the Authenticator object.
   *
   * It returns an authenticated client session via getClient().
   *
   * @param \Walkthrough\Authenticator\WalkthroughAuthenticator $authenticator
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

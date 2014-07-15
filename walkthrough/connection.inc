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
   * @param stromg $type
   *   Entity type to render.
   *   "walkthrough" or "walkthrough_set"
   * @param string $uuid
   *   Universally unique ID.
   * @param string|bool $extend_custom_class
   *   Custom class to extend, or FALSE to extend the default webdriver
   *  selenium class.
   *
   * @return string
   *   PHPUnit script.
   */
  public function getPhpunit($type, $uuid, $extend_custom_class = FALSE) {
    $client = $this->authenticator->getClient();

    if ($type == 'walkthrough') {
      $get_url = 'walkthrough-phpunit/' . $uuid;
    }
    else {
      $get_url = 'walkthrough-set-phpunit/' . $uuid;
    }

    if ($extend_custom_class) {
      $get_url .= '/' . $extend_custom_class;
    }

    $response = $client->get($get_url)->send()->json();

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
    $token = $this->getToken($client);

    $client->put('walkthrough-screening-flag/' . $uuid,
      array('X-CSRF-TOKEN' => $token),
      array('flag' => $flag_value)
    )->send()->json();
  }

  /**
   * Creates an empty screening node for a walkthrough.
   *
   * @param array $walkthrough
   *   Walkthrough structure.
   *   @see \\Walkthrough\Connection::getScreeningQueue()
   *
   * @return string
   *   UUID for the screening.
   */
  public function createEmptyScreening($walkthrough) {
    $client = $this->authenticator->getClient();
    $token = $this->getToken($client);

    $post_data = array(
      'title' => $walkthrough['title'],
      'referenced_walkthrough' => $walkthrough['uuid'],
    );

    $response = $client->post('walkhub-screening',
      array('X-CSRF-TOKEN' => $token),
      $post_data
    )->send()->json();

    return $response[0];
  }

  /**
   * Post back the screening results.
   *
   * @param string $screening_uuid
   *   UUID of the screening node to post the results to.
   *
   * @param array $result
   *   Screening result.
   *   Possible keys:
   *   - result: If the screening was successful or not.
   *   - junit: Junit result.
   *   - time: Time it took to take the screenshots.
   */
  public function sendScreeningResult($screening_uuid, $result) {
    $client = $this->authenticator->getClient();
    $token = $this->getToken($client);

    $client->put('walkhub-screening/' . $screening_uuid,
      array('X-CSRF-TOKEN' => $token),
      $result
    )->send()->json();
  }

  /**
   * Upload a screenshot for a screening.
   *
   * @param string $uuid
   *   UUID of the screening node to post the screenshots to.
   *
   * @param string $screenshot
   *   Filename of the screenshot.
   *
   * @param int|NULL $step
   *   (Optional) Step id.
   *   If not provided the server side will try to guess the step.
   *
   * @return string
   *   Query response.
   */
  public function uploadScreeningScreenshot($uuid, $screenshot, $step = NULL) {
    $client = $this->authenticator->getClient();
    $token = $this->getToken($client);

    $request = $client->put('walkhub-screening/' . $uuid, array('X-CSRF-TOKEN' => $token),
    array(
      'screenshots' => array(
        array(
          'step' => $step,
          'file' => base64_encode(file_get_contents($screenshot)),
        ),
      ),
    ));

    $response = $request->send();
    return $response;
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
   *   Rest endpoint.
   */
  public function setEndpoint($endpoint) {
    $this->endpoint = $endpoint;
  }

  /**
   * Get a drupal token.
   *
   * @param \Guzzle\Http\Client $client
   *   Guzzle http client connection.
   *
   * @return string
   *   Drupal token.
   */
  public function getToken($client) {
    $response = $client->post('user/token')->send()->json();
    $token = $response['token'];

    return $token;
  }
}

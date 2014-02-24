<?php

namespace Walkthrough\Authenticator\Oauth;

use Guzzle\Http\Message\RequestInterface;
use Guzzle\Plugin\Oauth\OauthPlugin;

/**
 * Custom Oauth implementation to accommodate Drupal issues.
 *
 * Class WalkhubOauthPlugin
 * @package Walkthrough\Authenticator\Oauth
 */
class WalkhubOauthPlugin extends OauthPlugin {

  /**
   * Overrides shouldPostFiedsBeSigned().
   *
   * Drupal uses the "official" Oauth library for PHP.
   * This library only uses the post fields to be signed
   * if the request method is POST.
   *
   * @param RequestInterface $request
   * @return bool
   */
  public function shouldPostFieldsBeSigned($request) {
    if ($request instanceof RequestInterface && $request->getMethod() === 'PUT') {
      return FALSE;
    }

    return parent::shouldPostFieldsBeSigned($request);
  }
}

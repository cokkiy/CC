import { Injectable, EventEmitter, Injector } from "@angular/core";
import { Http, Headers, RequestOptions, Response } from "@angular/http";
import { Observable } from "rxjs/Rx";
import "rxjs/add/observable/of";
import "rxjs/add/operator/do";
import "rxjs/add/operator/delay";

import { UserManager, Log, MetadataService, User } from "oidc-client";

@Injectable()
export class AuthService {
  private mgr: UserManager = new UserManager(settings);
  private userLoadededEvent: EventEmitter<User> = new EventEmitter<User>();
  currentUser: User;
  loggedIn: boolean = false;

  constructor() {
    // console.log("ctor AuthService");
    this.mgr.events.addUserUnloaded(e => {
      if (process.env.ENV === "development") {
        console.log("user unloaded");
      }
      this.loggedIn = false;
    });

    this.checkLogin();
  }

  // 检查是否已经登录
  async checkLogin(): Promise<boolean> {
    try {
      let user = await this.mgr.getUser();
      if (user) {
        this.loggedIn = true;
        this.currentUser = user;
        this.userLoadededEvent.emit(user);
      }
    } catch (ex) {
      this.loggedIn = false;
    }

    return this.loggedIn;
  }

  clearState() {
    this.mgr
      .clearStaleState()
      .then(function() {
        console.log("clearStateState success");
      })
      .catch(function(e) {
        console.log("clearStateState error", e.message);
      });
  }

  get currentUserName(): string {
    if (this.loggedIn) return this.currentUser.profile.name;
    return "";
  }

  //存储目的Url
  set redirectUrl(url: string) {
    sessionStorage.setItem("redirectUrl", url);
  }

  //获取将要导航的url
  get redirectUrl(): string {
    return sessionStorage.getItem("redirectUrl");
  }

  getUser() {
    this.mgr
      .getUser()
      .then(user => {
        console.log("got user", user);
        this.userLoadededEvent.emit(user);
      })
      .catch(function(err) {
        console.log(err);
      });
  }

  removeUser() {
    this.mgr
      .removeUser()
      .then(() => {
        this.userLoadededEvent.emit(null);
        console.log("user removed");
      })
      .catch(function(err) {
        console.log(err);
      });
  }

  startSigninMainWindow() {
    this.mgr
      .signinRedirect(/*{ data: 'some data' }*/)
      .then(function() {
        console.log("signinRedirect done");
      })
      .catch(function(err) {
        console.log(err);
      });
  }
  endSigninMainWindow() {
    return this.mgr
      .signinRedirectCallback()
      .then(user => {
        if (user) {
          this.loggedIn = true;
          this.currentUser = user;
          this.userLoadededEvent.emit(user);
          //console.log("signed in", user);
        }
      })
      .catch(function(err) {
        console.log(err);
      });
  }

  startSignoutMainWindow() {
    this.mgr
      .signoutRedirect()
      .then(function(resp) {
        console.log("signed out", resp);
        setTimeout(5000, () => {
          console.log("testing to see if fired...");
        });
      })
      .catch(function(err) {
        console.log(err);
      });
  }

  endSignoutMainWindow() {
    this.mgr
      .signoutRedirectCallback()
      .then(resp => {
        console.log("signed out", resp);
        this.removeUser();
      })
      .catch(err => {
        console.log(err);
      });
  }
}

// const settings: any = {
//    authority: 'https://passport.xmannal.com',
//    client_id: 'SourceTraceWebClient',
//    redirect_uri: 'https://www.xmannal.com/login',
//    post_logout_redirect_uri: 'https://www.xmannal.com/',
//    response_type: 'id_token token',
//    scope: 'openid profile FoodSourceTraceAPI.Manager',

//    silent_redirect_uri: 'https://www.xmannal.com/',
//    automaticSilentRenew: true,
//    //silentRequestTimeout:10000,

//    filterProtocolClaims: true,
//    loadUserInfo: true
// };

const settings: any = {
  authority: "http://localhost:5500",
  client_id: "SourceTraceWebClient",
  redirect_uri: "http://localhost:3000/login",
  post_logout_redirect_uri: "http://localhost:3000/",
  response_type: "id_token token",
  scope: "openid profile FoodSourceTraceAPI.Manager",

  silent_redirect_uri: "http://localhost:3000/",
  automaticSilentRenew: true,
  //silentRequestTimeout:10000,

  filterProtocolClaims: true,
  loadUserInfo: true
};

import { Component, AfterViewInit } from "@angular/core";

@Component({
  selector: "my-app",
  templateUrl: "app.component.html",
  styleUrls: ["app.component.scss"]
})
export class AppComponent implements AfterViewInit {
  ngAfterViewInit(): void {
    (<any>$(".navbar-sidenav [data-toggle=\"tooltip\"]")).tooltip({
      template:
        `<div class="tooltip navbar-sidenav-tooltip" role="tooltip" style="pointer-events: none;"><div class="arrow"></div><div class="tooltip-inner"></div></div>`
    });

    // force the toggled class to be removed when a collapsible nav link is clicked
    $(".navbar-sidenav .nav-link-collapse").click(function(e) {
      e.preventDefault();
      $("body").removeClass("sidenav-toggled");
    });
    // prevent the content wrapper from scrolling when the fixed side navigation hovered over
    $(
      "body.fixed-nav .navbar-sidenav, body.fixed-nav .sidenav-toggler, body.fixed-nav .navbar-collapse"
    ).on("mousewheel DOMMouseScroll", function(e) {
      var e0 = <any>e.originalEvent,
        delta = e0.wheelDelta || -e0.detail;
      this.scrollTop += (delta < 0 ? 1 : -1) * 30;
      e.preventDefault();
    });
    // scroll to top button appear
    $(document).scroll(function() {
      var scrollDistance = $(this).scrollTop();
      if (scrollDistance > 100) {
        $(".scroll-to-top").fadeIn();
      } else {
        $(".scroll-to-top").fadeOut();
      }
    });
    // configure tooltips globally
    (<any>$("[data-toggle=\"tooltip\"]")).tooltip();

    // smooth scrolling using jQuery easing
    $(document).on("click", "a.scroll-to-top", function(event:any):void {
      var $anchor = $(this);
      $("html, body")
        .stop()
        .animate(
          {
            scrollTop: $($anchor.attr("href")).offset().top
          },
          1000,
          "easeInOutExpo"
        );
      event.preventDefault();
    });
  }

  sidenavToggler():boolean {
    $("body").toggleClass("sidenav-toggled");
    $(".navbar-sidenav .nav-link-collapse").addClass("collapsed");
    $(
      ".navbar-sidenav .sidenav-second-level, .navbar-sidenav .sidenav-third-level"
    ).removeClass("show");
    return false;
  }
}

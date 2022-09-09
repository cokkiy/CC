import { Component, OnInit, Input, Output, EventEmitter } from '@angular/core';

class PageItem {
    constructor(public index: number, public enabled: boolean = true) {
    }
}

@Component({
    selector: 'pagination',
    templateUrl: './pagination.component.html',
    styleUrls: ['./pagination.component.css']
})
export class Pagination implements OnInit {
    @Output() pageSelected = new EventEmitter<number>();
    
    @Input() pageSize: number; //每页显示条目
    //分页标签数量，默认5
    @Input() navCount: number = 5;

    private pages: PageItem[] = [];
    private totalPage: number;
    private currentStartIndex: number = 0;
    private currentPageIndex: number = 1; //从1开始
    ngOnInit() {
        this.totalPage = Math.floor(this.total / this.pageSize)
            + (this.total % this.pageSize == 0 ? 0 : 1);
        this.buildPages();
    }

    private _total: number;
    public get total(): number {
        return this._total;
    }

    @Input()
    public set total(total: number) {
        this._total = total;
        this.totalPage = Math.floor(this.total / this.pageSize)
            + (this.total % this.pageSize == 0 ? 0 : 1);        
        this.currentPageIndex = 1;
        this.buildPages();
    }

    private buildPages() {
        this.pages.splice(0);
        for (let i = 0; i < this.navCount; i++) {
            let pageIndex = this.currentStartIndex + i + 1;
            let enabled = pageIndex <= this.totalPage;
            this.pages.push(new PageItem(pageIndex, enabled));
        }
    }

    private prevClicked(): boolean {
        if (this.currentPageIndex === 1)
            return false;
        this.currentPageIndex--;
        if (this.currentPageIndex - 1 < this.currentStartIndex){
            this.currentStartIndex -= this.pageSize;
            this.buildPages();
        }
        this.pageSelected.emit(this.currentPageIndex);
        return false;
    }

    private nextClicked(): boolean {
        if (this.currentPageIndex === this.totalPage)
            return false;
        this.currentPageIndex++;
        if (this.currentPageIndex > this.currentStartIndex + this.pageSize) {
            this.currentStartIndex += this.pageSize;
            this.buildPages();
        }
        this.pageSelected.emit(this.currentPageIndex);
        return false;
    }

    private pageClicked(pageItem: PageItem) {
        this.currentPageIndex = pageItem.index;
        this.pageSelected.emit(this.currentPageIndex);
        return false;
    }

    private setState(pageItem: PageItem) {
        let classes = {
            "active": pageItem.index === this.currentPageIndex,
            "disabled": !pageItem.enabled,
        };
        return classes;
    }

    private pageKeyup(pageIndex: number) {
        if (pageIndex && pageIndex != this.currentPageIndex
            && pageIndex <= this.totalPage && pageIndex > 0) {            
            this.currentPageIndex = pageIndex;
            this.currentPageIndex++;
            this.currentPageIndex--;
            this.currentStartIndex = Math.floor((this.currentPageIndex - 1) / this.pageSize) * this.pageSize;
            this.buildPages();
            this.pageSelected.emit(this.currentPageIndex);
        }
    }

    private pageGoto(pageIndex: number) {
        if (pageIndex && pageIndex != this.currentPageIndex
            && pageIndex <= this.totalPage && pageIndex > 0) {
            this.currentPageIndex = pageIndex;  
            this.currentPageIndex++;
            this.currentPageIndex--;          
            this.currentStartIndex = Math.floor((this.currentPageIndex - 1) / this.pageSize) * this.pageSize;
            this.buildPages();
            this.pageSelected.emit(this.currentPageIndex);
        }
    }


}
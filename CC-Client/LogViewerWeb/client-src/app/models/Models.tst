${
    Template(Settings settings)
    {
        settings
            .IncludeCurrentProject()
            .IncludeProject("StationLogModel");
        
        settings.OutputExtension = ".ts";
    }
}
module Models {
    $Classes(StationLogModels.*)[
    export class $Name {
        $Properties[
            public $name: $Type;]
    }]
    $Classes(LogViewerWeb.Models.ViewModels.*)[
    export class $Name {
        $Properties[
            public $name: $Type;]
    }]
    $Enums(LogViewerWeb.Models.ViewModels.*)[
    export enum $Name {
        $Values[
            $Name,]
    }]
    $Enums(StationLogModels.*)[
    export enum $Name {
        $Values[
            $Name,]
    }]
}